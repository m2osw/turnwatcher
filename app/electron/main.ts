import { app, BrowserWindow, ipcMain, dialog, Menu, MenuItemConstructorOptions, nativeTheme, screen } from 'electron'
import fs from 'node:fs'
import path from 'node:path'
import { fileManager } from './fileManager'

process.env.DIST_ELECTRON = path.join(__dirname)
process.env.DIST = path.join(process.env.DIST_ELECTRON, '../dist')
process.env.VITE_PUBLIC = process.env.VITE_DEV_SERVER_URL
  ? path.join(process.env.DIST_ELECTRON, '../public')
  : process.env.DIST

let mainWindow: BrowserWindow | null = null
let hudWindow: BrowserWindow | null = null

// Autosave file path — stored in the platform's user data directory
function getAutosavePath(): string {
  return path.join(app.getPath('userData'), 'turnwatcher-state.json')
}

function sendToRenderer(channel: string, ...args: any[]) {
  if (mainWindow && !mainWindow.isDestroyed()) {
    mainWindow.webContents.send(channel, ...args)
  }
}

// Return the zoom factor that makes the app look correctly sized for the
// current display's DPI scaling.
//
// • macOS: the OS compositor already handles HiDPI scaling; Chromium receives
//   logical pixels, so we always use 1.0 and let macOS do the rest.
// • Windows / Linux: the OS reports a scaleFactor (e.g. 1.25 for 125% DPI).
//   Chromium does NOT apply this automatically for Electron apps, so we set it
//   as the initial zoomFactor so the UI is sized to match the system expectation.
function getZoomFactor(): number {
  if (process.platform === 'darwin') return 1.0
  const primary = screen.getPrimaryDisplay()
  return primary.scaleFactor ?? 1.0
}

function createMainWindow() {
  const zoomFactor = getZoomFactor()

  mainWindow = new BrowserWindow({
    width: 900,
    height: 670,
    icon: path.join(__dirname, '../build/icon.png'),
    title: 'Turn Watcher',
    // Match the OS background colour so there is no white/black flash while
    // the renderer loads.  nativeTheme.shouldUseDarkColors reflects the
    // current macOS / Windows / Linux dark-mode preference.
    backgroundColor: nativeTheme.shouldUseDarkColors ? '#1e1e1e' : '#f0f0f0',
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
      zoomFactor,
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
    backgroundColor: nativeTheme.shouldUseDarkColors ? '#1e1e1e' : '#f0f0f0',
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
      zoomFactor: getZoomFactor(),
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

// ============================================================================
// Native Application Menu
// ============================================================================

function buildAppMenu() {
  const isMac = process.platform === 'darwin'

  const template: MenuItemConstructorOptions[] = [
    // macOS app menu
    ...(isMac ? [{
      label: app.name,
      submenu: [
        { role: 'about' as const },
        { type: 'separator' as const },
        {
          label: 'Preferences...',
          accelerator: 'CmdOrCtrl+,',
          click: () => sendToRenderer('menu:action', 'preferences'),
        },
        { type: 'separator' as const },
        { role: 'services' as const },
        { type: 'separator' as const },
        { role: 'hide' as const },
        { role: 'hideOthers' as const },
        { role: 'unhide' as const },
        { type: 'separator' as const },
        { role: 'quit' as const },
      ],
    }] : []),

    // File menu
    {
      label: 'File',
      submenu: [
        {
          label: 'Import...',
          accelerator: 'CmdOrCtrl+O',
          click: () => sendToRenderer('menu:action', 'file:import'),
        },
        {
          label: 'Export...',
          accelerator: 'CmdOrCtrl+Shift+S',
          click: () => sendToRenderer('menu:action', 'file:export'),
        },
        { type: 'separator' },
        {
          label: 'Clear All',
          click: () => sendToRenderer('menu:action', 'file:clear'),
        },
        { type: 'separator' },
        ...(!isMac ? [
          {
            label: 'Preferences...',
            click: () => sendToRenderer('menu:action', 'preferences'),
          },
          { type: 'separator' as const },
        ] : []),
        {
          label: 'Stat Manager...',
          click: () => sendToRenderer('menu:action', 'statManager'),
        },
        { type: 'separator' },
        isMac ? { role: 'close' as const } : { role: 'quit' as const },
      ],
    },

    // Edit menu
    {
      label: 'Edit',
      submenu: [
        {
          label: 'Undo',
          accelerator: 'CmdOrCtrl+Z',
          click: () => sendToRenderer('menu:action', 'edit:undo'),
        },
        {
          label: 'Redo',
          accelerator: isMac ? 'Cmd+Shift+Z' : 'Ctrl+Y',
          click: () => sendToRenderer('menu:action', 'edit:redo'),
        },
        { type: 'separator' },
        {
          label: 'Add Character...',
          accelerator: 'CmdOrCtrl+A',
          click: () => sendToRenderer('menu:action', 'edit:add'),
        },
        {
          label: 'Edit Character...',
          accelerator: 'CmdOrCtrl+E',
          click: () => sendToRenderer('menu:action', 'edit:edit'),
        },
        {
          label: 'Delete Character',
          accelerator: 'Delete',
          click: () => sendToRenderer('menu:action', 'edit:delete'),
        },
        {
          label: 'Duplicate Character',
          click: () => sendToRenderer('menu:action', 'edit:duplicate'),
        },
        { type: 'separator' },
        {
          label: 'Purge Dead',
          click: () => sendToRenderer('menu:action', 'edit:purgeDead'),
        },
      ],
    },

    // View menu
    {
      label: 'View',
      submenu: [
        {
          label: 'Show Player HUD',
          type: 'checkbox',
          checked: false,
          click: (menuItem) => {
            if (menuItem.checked) {
              createHUDWindow()
            } else {
              if (hudWindow) {
                hudWindow.close()
                hudWindow = null
              }
            }
            sendToRenderer('menu:action', 'view:toggleHUD')
          },
        },
        { type: 'separator' },
        { role: 'reload' },
        { role: 'forceReload' },
        { role: 'toggleDevTools' },
        { type: 'separator' },
        { role: 'resetZoom' },
        { role: 'zoomIn' },
        { role: 'zoomOut' },
        { type: 'separator' },
        { role: 'togglefullscreen' },
      ],
    },

    // Rounds menu
    {
      label: 'Rounds',
      submenu: [
        {
          label: 'Start',
          click: () => sendToRenderer('menu:action', 'rounds:start'),
        },
        {
          label: 'End',
          click: () => sendToRenderer('menu:action', 'rounds:end'),
        },
        {
          label: 'Next',
          accelerator: 'CmdOrCtrl+N',
          click: () => sendToRenderer('menu:action', 'rounds:next'),
        },
        { type: 'separator' },
        {
          label: 'Delay',
          click: () => sendToRenderer('menu:action', 'rounds:delay'),
        },
        {
          label: 'Ready',
          click: () => sendToRenderer('menu:action', 'rounds:ready'),
        },
        {
          label: 'Jump In...',
          click: () => sendToRenderer('menu:action', 'rounds:jumpIn'),
        },
        { type: 'separator' },
        {
          label: 'Damage...',
          accelerator: 'CmdOrCtrl+D',
          click: () => sendToRenderer('menu:action', 'rounds:damage'),
        },
        {
          label: 'Stabilize',
          click: () => sendToRenderer('menu:action', 'rounds:stabilize'),
        },
        { type: 'separator' },
        {
          label: 'Move Up',
          click: () => sendToRenderer('menu:action', 'rounds:moveUp'),
        },
        {
          label: 'Move Down',
          click: () => sendToRenderer('menu:action', 'rounds:moveDown'),
        },
      ],
    },

    // Roll menu
    {
      label: 'Roll',
      submenu: [
        {
          label: 'Initiative...',
          accelerator: 'CmdOrCtrl+I',
          click: () => sendToRenderer('menu:action', 'roll:initiative'),
        },
        { type: 'separator' },
        {
          label: 'Spot',
          click: () => sendToRenderer('menu:action', 'roll:stat:SPOT_ID'),
        },
        {
          label: 'Listen',
          click: () => sendToRenderer('menu:action', 'roll:stat:LISTEN_ID'),
        },
        {
          label: 'Will',
          click: () => sendToRenderer('menu:action', 'roll:stat:WILL_ID'),
        },
      ],
    },

    // Help menu
    {
      label: 'Help',
      role: 'help',
      submenu: [
        {
          label: 'About Turn Watcher',
          click: () => sendToRenderer('menu:action', 'about'),
        },
        { type: 'separator' },
        {
          label: 'Turn Watcher Website',
          click: () => {
            const { shell } = require('electron')
            shell.openExternal('https://www.turnwatcher.com')
          },
        },
        {
          label: 'Documentation',
          click: () => {
            const { shell } = require('electron')
            shell.openExternal('https://www.turnwatcher.com/documentation')
          },
        },
      ],
    },
  ]

  const menu = Menu.buildFromTemplate(template)
  Menu.setApplicationMenu(menu)
}

// ============================================================================
// IPC Handlers — File Import/Export
// ============================================================================

ipcMain.handle('file:import', async () => {
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

ipcMain.handle('file:export', async (_event, data: string) => {
  const result = await dialog.showSaveDialog(mainWindow!, {
    filters: [
      { name: 'Turn Watcher JSON', extensions: ['json'] },
    ],
    defaultPath: 'encounter.json',
  })
  if (result.canceled || !result.filePath) return null
  return fileManager.saveFile(result.filePath, data)
})

// ============================================================================
// IPC Handlers — Autosave (crash recovery)
// ============================================================================

ipcMain.handle('autosave:write', (_event, data: string) => {
  try {
    fs.writeFileSync(getAutosavePath(), data, 'utf-8')
    return true
  } catch {
    return false
  }
})

ipcMain.handle('autosave:read', () => {
  try {
    const filePath = getAutosavePath()
    if (!fs.existsSync(filePath)) return null
    const content = fs.readFileSync(filePath, 'utf-8')
    return content
  } catch {
    return null
  }
})

ipcMain.handle('autosave:clear', () => {
  try {
    const filePath = getAutosavePath()
    if (fs.existsSync(filePath)) {
      fs.unlinkSync(filePath)
    }
    return true
  } catch {
    return false
  }
})

// ============================================================================
// IPC Handlers — HUD Window
// ============================================================================

ipcMain.handle('hud:open', () => {
  createHUDWindow()
})

ipcMain.handle('hud:close', () => {
  if (hudWindow) {
    hudWindow.close()
    hudWindow = null
  }
})

// ============================================================================
// App Lifecycle
// ============================================================================

app.whenReady().then(() => {
  buildAppMenu()
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
