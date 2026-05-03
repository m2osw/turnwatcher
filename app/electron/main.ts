import { app, BrowserWindow, ipcMain, dialog } from 'electron'
import path from 'node:path'
import { fileManager } from './fileManager'

process.env.DIST_ELECTRON = path.join(__dirname)
process.env.DIST = path.join(process.env.DIST_ELECTRON, '../dist')
process.env.VITE_PUBLIC = process.env.VITE_DEV_SERVER_URL
  ? path.join(process.env.DIST_ELECTRON, '../public')
  : process.env.DIST

let mainWindow: BrowserWindow | null = null
let hudWindow: BrowserWindow | null = null

function createMainWindow() {
  mainWindow = new BrowserWindow({
    width: 900,
    height: 670,
    icon: path.join(__dirname, '../build/icon.png'),
    title: 'Turn Watcher',
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  })

  if (process.env.VITE_DEV_SERVER_URL) {
    mainWindow.loadURL(process.env.VITE_DEV_SERVER_URL)
  } else {
    mainWindow.loadFile(path.join(process.env.DIST!, 'index.html'))
  }

  mainWindow.on('closed', () => {
    mainWindow = null
    if (hudWindow) {
      hudWindow.close()
      hudWindow = null
    }
  })
}

function createHUDWindow() {
  if (hudWindow) {
    hudWindow.focus()
    return
  }

  hudWindow = new BrowserWindow({
    width: 600,
    height: 400,
    title: 'Turn Watcher - Player HUD',
    icon: path.join(__dirname, '../build/icon.png'),
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  })

  if (process.env.VITE_DEV_SERVER_URL) {
    hudWindow.loadURL(`${process.env.VITE_DEV_SERVER_URL}#/hud`)
  } else {
    hudWindow.loadFile(path.join(process.env.DIST!, 'index.html'), {
      hash: '/hud',
    })
  }

  hudWindow.on('closed', () => {
    hudWindow = null
  })
}

// IPC Handlers
ipcMain.handle('file:open', async () => {
  const result = await dialog.showOpenDialog(mainWindow!, {
    filters: [
      { name: 'Turn Watcher Files', extensions: ['json', 'turnwatcher'] },
      { name: 'JSON Files', extensions: ['json'] },
      { name: 'Legacy Files', extensions: ['turnwatcher'] },
      { name: 'All Files', extensions: ['*'] },
    ],
    properties: ['openFile'],
  })
  if (result.canceled || result.filePaths.length === 0) return null
  return fileManager.loadFile(result.filePaths[0])
})

ipcMain.handle('file:save', async (_event, data: string, filePath?: string) => {
  if (!filePath) {
    const result = await dialog.showSaveDialog(mainWindow!, {
      filters: [
        { name: 'Turn Watcher JSON', extensions: ['json'] },
      ],
      defaultPath: 'encounter.json',
    })
    if (result.canceled || !result.filePath) return null
    filePath = result.filePath
  }
  return fileManager.saveFile(filePath, data)
})

ipcMain.handle('hud:open', () => {
  createHUDWindow()
})

ipcMain.handle('hud:close', () => {
  if (hudWindow) {
    hudWindow.close()
    hudWindow = null
  }
})

app.whenReady().then(() => {
  createMainWindow()

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createMainWindow()
    }
  })
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})
