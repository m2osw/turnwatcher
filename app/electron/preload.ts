import { contextBridge, ipcRenderer } from 'electron'

contextBridge.exposeInMainWorld('electronAPI', {
  fileOpen: () => ipcRenderer.invoke('file:open'),
  fileSave: (data: string, filePath?: string) =>
    ipcRenderer.invoke('file:save', data, filePath),
  hudOpen: () => ipcRenderer.invoke('hud:open'),
  hudClose: () => ipcRenderer.invoke('hud:close'),

  // Native menu action listener
  onMenuAction: (callback: (action: string) => void) => {
    const handler = (_event: any, action: string) => callback(action)
    ipcRenderer.on('menu:action', handler)
    // Return cleanup function
    return () => ipcRenderer.removeListener('menu:action', handler)
  },
})
