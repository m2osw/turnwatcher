import { createSlice, PayloadAction } from '@reduxjs/toolkit'
import { InitiativeState } from '../../types'

const initialState: InitiativeState = {
  inRounds: false,
  roundNumber: 0,
  currentInit: 0,
}

const initiativeSlice = createSlice({
  name: 'initiative',
  initialState,
  reducers: {
    startRounds(state) {
      state.inRounds = true
      state.roundNumber = 1
      state.currentInit = 1
    },

    endRounds(state) {
      state.inRounds = false
      state.roundNumber = 0
      state.currentInit = 0
    },

    nextTurn(state, action: PayloadAction<{ nextInit: number; roundNumber: number }>) {
      state.currentInit = action.payload.nextInit
      state.roundNumber = action.payload.roundNumber
    },

    setCurrentInit(state, action: PayloadAction<number>) {
      state.currentInit = action.payload
    },

    setRoundNumber(state, action: PayloadAction<number>) {
      state.roundNumber = action.payload
    },

    setInitiativeState(_state, action: PayloadAction<InitiativeState>) {
      return action.payload
    },
  },
})

export const {
  startRounds,
  endRounds,
  nextTurn,
  setCurrentInit,
  setRoundNumber,
  setInitiativeState,
} = initiativeSlice.actions

export default initiativeSlice.reducer
