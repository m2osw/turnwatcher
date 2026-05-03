import { contextBridge, ipcRenderer } from 'electron'

contextBridge.exposeInMainWorld('electronAPI', {
  fileOpen: () => ipcRenderer.invoke('file:open'),
  fileSave: (data: string, filePath?: string) =>
    ipcRenderer.invoke('file:save', data, filePath),
  hudOpen: () => ipcRenderer.invoke('hud:open'),
  hudClose: () => ipcRenderer.invoke('hud:close'),
})
