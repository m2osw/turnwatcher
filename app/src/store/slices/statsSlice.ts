import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import { StatDefinition } from '../../types'

// Default stat definitions matching the C++ StatManager::CreateDefaultStats()
const defaultStats: StatDefinition[] = [
  { id: 'INIT_ID', name: 'Initiative', abilityId: 'DEX_ID', dice: 1, faces: 20, modifier: 0, accel: 'I', showOnToolbar: true, showOnHUD: false, showMonsterOnHUD: false, internal: false, ability: false, order: 0, deleted: false },
  { id: 'HITPOINTS_ID', name: 'Hit Points', abilityId: 'NONE', dice: 1, faces: 20, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: true, ability: false, order: 1, deleted: false },
  { id: 'STR_ID', name: 'Strength', abilityId: 'NONE', dice: 1, faces: 20, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: false, ability: true, order: 2, deleted: false },
  { id: 'DEX_ID', name: 'Dexterity', abilityId: 'NONE', dice: 1, faces: 20, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: false, ability: true, order: 3, deleted: false },
  { id: 'CON_ID', name: 'Constitution', abilityId: 'NONE', dice: 1, faces: 20, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: false, ability: true, order: 4, deleted: false },
  { id: 'INT_ID', name: 'Intelligence', abilityId: 'NONE', dice: 1, faces: 20, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: false, ability: true, order: 5, deleted: false },
  { id: 'WIS_ID', name: 'Wisdom', abilityId: 'NONE', dice: 1, faces: 20, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: false, ability: true, order: 6, deleted: false },
  { id: 'CHA_ID', name: 'Charisma', abilityId: 'NONE', dice: 1, faces: 20, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: false, ability: true, order: 7, deleted: false },
  { id: 'SPOT_ID', name: 'Spot', abilityId: 'WIS_ID', dice: 1, faces: 20, modifier: 0, accel: 'S', showOnToolbar: true, showOnHUD: true, showMonsterOnHUD: false, internal: false, ability: false, order: 8, deleted: false },
  { id: 'LISTEN_ID', name: 'Listen', abilityId: 'WIS_ID', dice: 1, faces: 20, modifier: 0, accel: 'L', showOnToolbar: true, showOnHUD: true, showMonsterOnHUD: false, internal: false, ability: false, order: 9, deleted: false },
  { id: 'WILL_ID', name: 'Will', abilityId: 'WIS_ID', dice: 1, faces: 20, modifier: 0, accel: 'W', showOnToolbar: true, showOnHUD: true, showMonsterOnHUD: false, internal: false, ability: false, order: 10, deleted: false },
  { id: 'LEVEL_ID', name: 'Level', abilityId: 'NONE', dice: 0, faces: 0, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: true, ability: false, order: 11, deleted: false },
  { id: 'FORTITUDE_ID', name: 'Fortitude', abilityId: 'CON_ID', dice: 1, faces: 20, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: false, ability: false, order: 12, deleted: false },
  { id: 'REFLEX_ID', name: 'Reflex', abilityId: 'DEX_ID', dice: 1, faces: 20, modifier: 0, accel: '', showOnToolbar: false, showOnHUD: false, showMonsterOnHUD: false, internal: false, ability: false, order: 13, deleted: false },
]

interface StatsState {
  definitions: StatDefinition[]
}

const initialState: StatsState = {
  definitions: defaultStats,
}

const statsSlice = createSlice({
  name: 'stats',
  initialState,
  reducers: {
    addStat(state, action: PayloadAction<StatDefinition>) {
      state.definitions.push(action.payload)
    },

    updateStat(state, action: PayloadAction<{ id: string; changes: Partial<StatDefinition> }>) {
      const idx = state.definitions.findIndex(s => s.id === action.payload.id)
      if (idx !== -1) {
        state.definitions[idx] = { ...state.definitions[idx], ...action.payload.changes }
      }
    },

    removeStat(state, action: PayloadAction<string>) {
      const idx = state.definitions.findIndex(s => s.id === action.payload)
      if (idx !== -1) {
        state.definitions[idx].deleted = true
      }
    },

    reorderStat(state, action: PayloadAction<{ id: string; newOrder: number }>) {
      const stat = state.definitions.find(s => s.id === action.payload.id)
      if (stat) {
        stat.order = action.payload.newOrder
      }
    },

    setStats(state, action: PayloadAction<StatDefinition[]>) {
      state.definitions = action.payload
    },

    resetToDefaults(state) {
      state.definitions = defaultStats
    },
  },
})

export const {
  addStat,
  updateStat,
  removeStat,
  reorderStat,
  setStats,
  resetToDefaults,
} = statsSlice.actions

export default statsSlice.reducer
