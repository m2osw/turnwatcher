import { configureStore } from '@reduxjs/toolkit'
import { TypedUseSelectorHook, useDispatch, useSelector } from 'react-redux'
import undoable from 'redux-undo'
import charactersReducer from './slices/charactersSlice'
import initiativeReducer from './slices/initiativeSlice'
import statsReducer from './slices/statsSlice'
import settingsReducer from './slices/settingsSlice'
import uiReducer from './slices/uiSlice'

export const store = configureStore({
  reducer: {
    characters: undoable(charactersReducer, {
      limit: 50,
      filter: (action) => {
        // Only track meaningful actions for undo, not UI-only changes
        return action.type.startsWith('characters/')
      },
    }),
    initiative: initiativeReducer,
    stats: statsReducer,
    settings: settingsReducer,
    ui: uiReducer,
  },
})

export type RootState = ReturnType<typeof store.getState>
export type AppDispatch = typeof store.dispatch

// Typed hooks
export const useAppDispatch: () => AppDispatch = useDispatch
export const useAppSelector: TypedUseSelectorHook<RootState> = useSelector
