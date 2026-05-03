import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import { Character, Effect, Status, StatValue } from '../../types'
import { v4 as uuidv4 } from 'uuid'

interface CharactersState {
  list: Character[]
}

const initialState: CharactersState = {
  list: [],
}

function createDefaultCharacter(): Character {
  return {
    id: uuidv4(),
    name: '',
    publicName: '',
    notes: '',
    monster: false,
    baseHP: 10,
    tempHP: 0,
    damage: 0,
    hitDice: 1,
    status: Status.Normal,
    stabilized: false,
    justDropped: false,
    position: 0,
    subPosition: 0,
    manualPos: 0,
    deleted: false,
    stats: {},
    effects: [],
  }
}

const charactersSlice = createSlice({
  name: 'characters',
  initialState,
  reducers: {
    addCharacter(state, action: PayloadAction<Partial<Character>>) {
      const ch = { ...createDefaultCharacter(), ...action.payload }
      if (!ch.id) ch.id = uuidv4()
      state.list.push(ch)
    },

    updateCharacter(state, action: PayloadAction<{ id: string; changes: Partial<Character> }>) {
      const idx = state.list.findIndex(c => c.id === action.payload.id)
      if (idx !== -1) {
        state.list[idx] = { ...state.list[idx], ...action.payload.changes }
      }
    },

    removeCharacter(state, action: PayloadAction<string>) {
      state.list = state.list.filter(c => c.id !== action.payload)
    },

    duplicateCharacter(state, action: PayloadAction<string>) {
      const source = state.list.find(c => c.id === action.payload)
      if (source) {
        const dup: Character = {
          ...JSON.parse(JSON.stringify(source)),
          id: uuidv4(),
          name: `${source.name} (copy)`,
        }
        state.list.push(dup)
      }
    },

    setCharacters(state, action: PayloadAction<Character[]>) {
      state.list = action.payload
    },

    clearCharacters(state) {
      state.list = []
    },

    purgeDead(state) {
      state.list = state.list.filter(c => {
        const hp = c.baseHP + c.tempHP - c.damage
        return hp > -c.baseHP // not irrevocably dead
      })
    },

    applyDamage(state, action: PayloadAction<{ id: string; amount: number }>) {
      const ch = state.list.find(c => c.id === action.payload.id)
      if (ch) {
        ch.damage += action.payload.amount
        if (ch.damage < 0) ch.damage = 0

        const hp = ch.baseHP + ch.tempHP - ch.damage
        if (hp <= 0 && !ch.justDropped) {
          ch.justDropped = true
        }
      }
    },

    healCharacter(state, action: PayloadAction<{ id: string; amount: number }>) {
      const ch = state.list.find(c => c.id === action.payload.id)
      if (ch) {
        ch.damage = Math.max(0, ch.damage - action.payload.amount)
        if (ch.damage === 0) {
          ch.justDropped = false
          ch.stabilized = false
        }
      }
    },

    fullHeal(state, action: PayloadAction<string>) {
      const ch = state.list.find(c => c.id === action.payload)
      if (ch) {
        ch.damage = 0
        ch.justDropped = false
        ch.stabilized = false
      }
    },

    setCharacterStatus(state, action: PayloadAction<{ id: string; status: Status }>) {
      const ch = state.list.find(c => c.id === action.payload.id)
      if (ch) {
        ch.status = action.payload.status
      }
    },

    stabilizeCharacter(state, action: PayloadAction<string>) {
      const ch = state.list.find(c => c.id === action.payload)
      if (ch) {
        ch.stabilized = !ch.stabilized
      }
    },

    setStatValue(state, action: PayloadAction<{ charId: string; statId: string; value: Partial<StatValue> }>) {
      const ch = state.list.find(c => c.id === action.payload.charId)
      if (ch) {
        const existing = ch.stats[action.payload.statId] || { mod: 0, roll: 0, notes: '' }
        ch.stats[action.payload.statId] = { ...existing, ...action.payload.value }
      }
    },

    addEffect(state, action: PayloadAction<{ charId: string; effect: Effect }>) {
      const ch = state.list.find(c => c.id === action.payload.charId)
      if (ch) {
        const effect = { ...action.payload.effect }
        if (!effect.id) effect.id = uuidv4()
        ch.effects.push(effect)
      }
    },

    updateEffect(state, action: PayloadAction<{ charId: string; effectId: string; changes: Partial<Effect> }>) {
      const ch = state.list.find(c => c.id === action.payload.charId)
      if (ch) {
        const idx = ch.effects.findIndex(e => e.id === action.payload.effectId)
        if (idx !== -1) {
          ch.effects[idx] = { ...ch.effects[idx], ...action.payload.changes }
        }
      }
    },

    removeEffect(state, action: PayloadAction<{ charId: string; effectId: string }>) {
      const ch = state.list.find(c => c.id === action.payload.charId)
      if (ch) {
        ch.effects = ch.effects.filter(e => e.id !== action.payload.effectId)
      }
    },

    advanceEffects(state, action: PayloadAction<string>) {
      const ch = state.list.find(c => c.id === action.payload)
      if (ch) {
        ch.effects = ch.effects.map(e => {
          if (e.startIn > 0) {
            return { ...e, startIn: e.startIn - 1 }
          }
          if (e.isActive) {
            return { ...e, roundsUsed: e.roundsUsed + 1 }
          }
          return e
        })
      }
    },
  },
})

export const {
  addCharacter,
  updateCharacter,
  removeCharacter,
  duplicateCharacter,
  setCharacters,
  clearCharacters,
  purgeDead,
  applyDamage,
  healCharacter,
  fullHeal,
  setCharacterStatus,
  stabilizeCharacter,
  setStatValue,
  addEffect,
  updateEffect,
  removeEffect,
  advanceEffects,
} = charactersSlice.actions

export default charactersSlice.reducer
