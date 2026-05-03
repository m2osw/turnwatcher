import { createSlice, PayloadAction } from '@reduxjs/toolkit'

export type DialogType =
  | 'editCharacter'
  | 'damageDialog'
  | 'initRollDialog'
  | 'effectsEditor'
  | 'statManager'
  | 'preferences'
  | 'aboutDialog'
  | 'duplicateResolver'
  | 'jumpIn'
  | 'defineInitDice'
  | 'exportDialog'
  | null

interface UIState {
  selectedCharacterIds: string[]
  openDialog: DialogType
  editingCharacterId: string | null
  editingEffectId: string | null
  currentFilePath: string | null
  isModified: boolean
  statusMessage: string
}

const initialState: UIState = {
  selectedCharacterIds: [],
  openDialog: null,
  editingCharacterId: null,
  editingEffectId: null,
  currentFilePath: null,
  isModified: false,
  statusMessage: '',
}

const uiSlice = createSlice({
  name: 'ui',
  initialState,
  reducers: {
    setSelectedCharacters(state, action: PayloadAction<string[]>) {
      state.selectedCharacterIds = action.payload
    },

    toggleCharacterSelection(state, action: PayloadAction<string>) {
      const idx = state.selectedCharacterIds.indexOf(action.payload)
      if (idx === -1) {
        state.selectedCharacterIds.push(action.payload)
      } else {
        state.selectedCharacterIds.splice(idx, 1)
      }
    },

    clearSelection(state) {
      state.selectedCharacterIds = []
    },

    openDialog(state, action: PayloadAction<{ dialog: DialogType; characterId?: string; effectId?: string }>) {
      state.openDialog = action.payload.dialog
      if (action.payload.characterId !== undefined) {
        state.editingCharacterId = action.payload.characterId
      }
      if (action.payload.effectId !== undefined) {
        state.editingEffectId = action.payload.effectId
      }
    },

    closeDialog(state) {
      state.openDialog = null
      state.editingCharacterId = null
      state.editingEffectId = null
    },

    setCurrentFilePath(state, action: PayloadAction<string | null>) {
      state.currentFilePath = action.payload
    },

    setModified(state, action: PayloadAction<boolean>) {
      state.isModified = action.payload
    },

    setStatusMessage(state, action: PayloadAction<string>) {
      state.statusMessage = action.payload
    },
  },
})

export const {
  setSelectedCharacters,
  toggleCharacterSelection,
  clearSelection,
  openDialog,
  closeDialog,
  setCurrentFilePath,
  setModified,
  setStatusMessage,
} = uiSlice.actions

export default uiSlice.reducer
