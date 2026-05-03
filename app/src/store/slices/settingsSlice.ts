import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import { AppSettings } from '../../types'

const initialState: AppSettings = {
  ultraInit: false,
  bleedOutDying: true,
  skipDead: true,
  altDeath: false,
  notifyExpiredEffects: true,
  showToolbar: true,
  rollInitOnStart: false,
  manualInitiative: false,
  getDC: false,
  lastDC: 10,
  showEffects: true,
  showInfo: true,
  showHUD: false,
  deathThreshold: -10,
  combatantListFont: '',
  altCombatantListFont: '',
  initDieStrings: ['1d20'],
  toolbarPos: 'top',
}

const settingsSlice = createSlice({
  name: 'settings',
  initialState,
  reducers: {
    updateSettings(state, action: PayloadAction<Partial<AppSettings>>) {
      return { ...state, ...action.payload }
    },

    setSettings(_state, action: PayloadAction<AppSettings>) {
      return action.payload
    },

    resetSettings() {
      return initialState
    },
  },
})

export const {
  updateSettings,
  setSettings,
  resetSettings,
} = settingsSlice.actions

export default settingsSlice.reducer
