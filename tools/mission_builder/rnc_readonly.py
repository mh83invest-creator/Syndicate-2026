"""Bounded, read-only RNC Method 1 decoder and mission summary utility."""
import argparse
import json
import re
import sys
from pathlib import PurePosixPath
from zipfile import ZipFile

class FormatError(ValueError): pass

def crc16(raw):
    v=0
    for b in raw:
        v^=b
        for _ in range(8):v=(v>>1)^ (0xA001 if v&1 else 0)
    return v

def decode(raw, limit=116010):
    if len(raw)<18 or raw[:4]!=b'RNC\x01':raise FormatError('RNC1 header missing')
    outlen=int.from_bytes(raw[4:8],'big'); packed=int.from_bytes(raw[8:12],'big')
    if outlen>limit or not outlen: raise FormatError('invalid expanded size')
    if len(raw)!=18+packed:raise FormatError('packed size mismatch')
    if crc16(raw[18:])!=int.from_bytes(raw[14:16],'big'):raise FormatError('packed CRC mismatch')
    payload=raw[18:]; plen=len(payload); blocks=raw[17]
    if not blocks:raise FormatError('zero blocks')
    p=0; bitbuf=0; count=0; out=bytearray()
    def word(pos):
        return int.from_bytes(payload[pos:pos+2].ljust(2,b'\0'),'little')
    bitbuf=word(0);count=16
    def adv(n):
        nonlocal p,bitbuf,count
        if n>count or n<0:raise FormatError('invalid bit request')
        bitbuf>>=n;count-=n
        if count<16:
            p+=2
            if p>plen+2:raise FormatError('bitstream truncated')
            bitbuf|=word(p)<<count
            count+=16
    def bits(n):
        v=bitbuf&((1<<n)-1);adv(n);return v
    def table():
        n=bits(5)
        if n>16:raise FormatError('excessive huffman symbols')
        lens=[bits(4) for _ in range(n)]
        c=0; entries={}
        for width in range(1,max(lens,default=0)+1):
            for idx, l in enumerate(lens):
                if l==width:
                    mirror=sum(((c>>k)&1)<<(width-1-k) for k in range(width))
                    entries[(width,mirror)]=idx;c+=1
            c<<=1
        if not entries:raise FormatError('empty huffman table')
        return entries
    def value(t):
        for width in range(1,16):
            key=(width,bitbuf&((1<<width)-1))
            if key in t:
                v=t[key];adv(width)
                if v>=2:v=(1<<(v-1)) | bits(v-1)
                return v
        raise FormatError('huffman decoding failed')
    adv(2)
    for block in range(blocks):
        if len(out)==outlen: break
        rawtable=table();dist=table();lentable=table();chunks=bits(16)
        if chunks==0:raise FormatError('empty chunk list')
        for chunk in range(chunks):
            length=value(rawtable)
            if (length and p+length>plen) or len(out)+length>outlen:raise FormatError('literal outside range')
            if length:
                out.extend(payload[p:p+length]);p+=length
                count-=16
                if count<0:raise FormatError('invalid literal bit alignment')
                bitbuf &= (1<<count)-1
                bitbuf |= word(p)<<count
                count+=16
            if chunk==chunks-1:break
            distance=value(dist)+1;copylen=value(lentable)+2
            if distance>len(out) or len(out)+copylen>outlen:raise FormatError('backreference outside range')
            for _ in range(copylen):out.append(out[-distance])
    if len(out)!=outlen:raise FormatError(f'expanded size mismatch {len(out)} != {outlen}')
    if crc16(out)!=int.from_bytes(raw[12:14],'big'):raise FormatError('expanded CRC mismatch')
    return bytes(out)

def inspect_archive(path):
    result=[]
    with ZipFile(path) as z:
        names=[n for n in z.namelist() if re.fullmatch(r'game\d\d\.dat',PurePosixPath(n).name.lower())]
        for name in sorted(names):
            data=z.read(name);rec={'file':PurePosixPath(name).name}
            try:
                unpacked=decode(data)
                rec.update(ok=True,expanded=len(unpacked),map_number=int.from_bytes(unpacked[0x1bd28:0x1bd2a],'little'),
                           objective_bytes=unpacked[0x1bd32:0x1bd32+112].hex()[:56])
            except FormatError as ex:rec.update(ok=False,error=str(ex))
            result.append(rec)
    return result

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('zipfile');p.add_argument('--json',action='store_true');a=p.parse_args()
    rows=inspect_archive(a.zipfile)
    if a.json:print(json.dumps(rows,indent=2))
    else:
        print(f"Decoded {sum(r['ok'] for r in rows)}/{len(rows)} mission files")
        for r in rows:
            if not r['ok']:print(r)
    sys.exit(0 if rows and all(r['ok'] for r in rows) else 1)
