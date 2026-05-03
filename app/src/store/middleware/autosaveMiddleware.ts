// Autosave middleware — mirrors the C++ SAVE_TIMEOUT_SECS = 10 pattern.
// Debounces state writes to the autosave file every 10 seconds after changes.

import { Middleware } from '@reduxjs/toolkit'
import { RootState } from '../index'
import { SaveFile } from '../../types'

const AUTOSAVE_DELAY_MS = 10_000 // 10 seconds, matching original C++

let autosaveTimer: ReturnType<typeof setTimeout> | null = null

// Prefixes of actions that should trigger an autosave
const TRACKED_PREFIXES = [
  'characters/',
  'initiative/',
  'stats/',
  'settings/',
]

function shouldAutosave(actionType: string): boolean {
  return TRACKED_PREFIXES.some(prefix => actionType.startsWith(prefix))
}

function serializeState(state: RootState): string {
  const saveFile: SaveFile = {
    version: '2.0',
    settings: state.settings,
    characters: state.characters.present.list,
    stats: state.stats.definitions,
    initiative: state.initiative,
  }
  return JSON.stringify(saveFile)
}

export const autosaveMiddleware: Middleware<{}, RootState> = (storeAPI) => (next) => (action: any) => {
  const result = next(action)

  if (action?.type && shouldAutosave(action.type)) {
    // Clear any pending autosave and schedule a new one
    if (autosaveTimer) {
      clearTimeout(autosaveTimer)
    }

    autosaveTimer = setTimeout(() => {
      autosaveTimer = null
      const state = storeAPI.getState()
      const data = serializeState(state)

      if (window.electronAPI?.autosaveWrite) {
        window.electronAPI.autosaveWrite(data)
      }
    }, AUTOSAVE_DELAY_MS)
  }

  return result
}

/**
 * Force an immediate autosave (called on app quit / before-unload).
 */
export function forceAutosave(state: RootState): void {
  if (autosaveTimer) {
    clearTimeout(autosaveTimer)
    autosaveTimer = null
  }

  const data = serializeState(state)
  if (window.electronAPI?.autosaveWrite) {
    window.electronAPI.autosaveWrite(data)
  }
}
