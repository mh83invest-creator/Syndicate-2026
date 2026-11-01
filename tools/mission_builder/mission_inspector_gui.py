"""Local read-only Syndicate mission inspector (Tkinter). Requires adjacent inspector scripts."""
import argparse
import re
import tkinter as tk
from pathlib import PurePosixPath
from tkinter import filedialog, messagebox, ttk
from zipfile import ZipFile
from mission_inspector import inspect_mission
from test_mission_entity_chains import examine_mission

WARNINGS = ('out_of_range_links','nonreciprocal_child_links','cycles','target_mismatches','entity_type_zero')

def load_archive(path):
    rows=[]
    with ZipFile(path) as archive:
        names=sorted(n for n in archive.namelist() if
                     re.fullmatch(r'game\d\d\.dat', PurePosixPath(n).name.lower()))
        for member in names:
            raw=archive.read(member)
            name=PurePosixPath(member).name.lower()
            entry=inspect_mission(raw,name)
            entry['diagnostics']=examine_mission(raw,name)
            rows.append(entry)
    if not rows: raise ValueError('No gameNN.dat mission resources in ZIP')
    return rows

class Inspector(tk.Tk):
    def __init__(self, archive=None):
        super().__init__()
        self.title('Syndicate 2026 — Mission Inspector (read-only)')
        self.geometry('1100x700')
        self.missions=[]
        header=ttk.Frame(self,padding=10);header.pack(fill='x')
        ttk.Button(header,text='Open mission ZIP…',command=self.choose).pack(side='left')
        self.status=tk.StringVar(value='Choose a ZIP containing original mission files.')
        ttk.Label(header,textvariable=self.status).pack(side='left',padx=10)
        panes=ttk.Panedwindow(self,orient='horizontal');panes.pack(fill='both',expand=True,padx=10,pady=10)
        left=ttk.Frame(panes);panes.add(left,weight=1)
        ttk.Label(left,text='Missions').pack(anchor='w')
        self.selection=tk.Listbox(left,exportselection=False,width=25)
        self.selection.pack(fill='both',expand=True)
        self.selection.bind('<<ListboxSelect>>',self.show)
        right=ttk.Frame(panes);panes.add(right,weight=5)
        self.caption=tk.StringVar(value='No mission selected')
        ttk.Label(right,textvariable=self.caption).pack(anchor='w')
        tabs=ttk.Notebook(right);tabs.pack(fill='both',expand=True)
        self.tables={}
        specs={
          'Tactical':('slot','status','type','target_offset','x','y','z_or_parameter'),
          'CP Objectives':('slot','child','parent','field_4','flags','action_type','action','target_offset','field_b','field_d'),
          'Diagnostics':('category','details')}
        for title,fields in specs.items():
            pane=ttk.Frame(tabs,padding=4);tabs.add(pane,text=title)
            tree=ttk.Treeview(pane,columns=fields,show='headings')
            for field in fields:
                tree.heading(field,text=field.replace('_',' ').title())
                tree.column(field,width=120 if field!='details' else 350)
            scroll=ttk.Scrollbar(pane,orient='vertical',command=tree.yview)
            tree.configure(yscrollcommand=scroll.set)
            tree.pack(side='left',fill='both',expand=True);scroll.pack(side='right',fill='y')
            self.tables[title]=(tree,fields)
        if archive:self.open(archive)

    def choose(self):
        path=filedialog.askopenfilename(filetypes=[('ZIP archives','*.zip')])
        if path:self.open(path)

    def open(self,path):
        try: rows=load_archive(path)
        except (OSError,ValueError,KeyError) as error:
            messagebox.showerror('Unable to inspect ZIP',str(error));return
        self.missions=rows;self.selection.delete(0,'end')
        for row in rows:
            warnings=sum(len(row['diagnostics'][k]) for k in WARNINGS)
            self.selection.insert('end',f"{row['file']}  map {row['map_number']}"+(f'  !{warnings}' if warnings else ''))
        self.status.set(f'{len(rows)} missions • read only')
        self.selection.selection_set(0);self.show()

    def fill(self,name,rows):
        tree,fields=self.tables[name]
        for item in tree.get_children():tree.delete(item)
        for row in rows:tree.insert('','end',values=[row.get(field,'') for field in fields])

    def show(self,_event=None):
        selected=self.selection.curselection()
        if not selected:return
        row=self.missions[selected[0]]
        warning=sum(len(row['diagnostics'][k]) for k in WARNINGS)
        self.caption.set(f"{row['file']}  |  Map {row['map_number']}  |  {row['decoded_bytes']:,} bytes  |  {warning} diagnostics")
        self.fill('Tactical',row['objectives'])
        active=[x for x in row['computer_player_objectives']
                if any(v for k,v in x.items() if k!='slot')]
        self.fill('CP Objectives',active)
        diagnostics=[{'category':key,'details':str(item)}
                     for key in WARNINGS for item in row['diagnostics'][key]]
        self.fill('Diagnostics',diagnostics)

if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('archive',nargs='?')
    args=parser.parse_args()
    Inspector(args.archive).mainloop()
