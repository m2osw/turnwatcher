import { contextBridge, ipcRenderer } from 'electron'

contextBridge.exposeInMainWorld('electronAPI', {
  // File Import/Export
  fileImport: () => ipcRenderer.invoke('file:import'),
  fileExport: (data: string) => ipcRenderer.invoke('file:export', data),

  // Autosave (crash recovery)
  autosaveWrite: (data: string) => ipcRenderer.invoke('autosave:write', data),
  autosaveRead: () => ipcRenderer.invoke('autosave:read'),
  autosaveClear: () => ipcRenderer.invoke('autosave:clear'),

  // HUD window
  hudOpen: () => ipcRenderer.invoke('hud:open'),
  hudClose: () => ipcRenderer.invoke('hud:close'),

  // Native menu action listener
  onMenuAction: (callback: (action: string) => void) => {
    const handler = (_event: any, action: string) => callback(action)
    ipcRenderer.on('menu:action', handler)
    return () => ipcRenderer.removeListener('menu:action', handler)
  },
})
