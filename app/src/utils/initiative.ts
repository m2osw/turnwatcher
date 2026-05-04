// Initiative sorting and management utilities
// Mirrors C++ Initiative::InitiativeManager logic

import { Character, Status, StatDefinition, AppSettings } from '../types'
import { getCurrentHP } from './health'
import { makeStatRoll } from './dice'

/**
 * Compare two characters for initiative ordering.
 * Sort by: position (descending), then subPosition (descending) for ties.
 * Characters with manualPos > 0 get that position instead.
 */
export function compareInitiative(a: Character, b: Character): number {
  const posA = a.manualPos > 0 ? a.manualPos : a.position
  const posB = b.manualPos > 0 ? b.manualPos : b.position

  // Higher initiative goes first
  if (posA !== posB) return posB - posA

  // Sub-position for tie-breaking
  return b.subPosition - a.subPosition
}

/**
 * Sort characters by initiative order.
 * Returns a new sorted array (does not mutate).
 */
export function sortByInitiative(characters: Character[]): Character[] {
  return [...characters].sort(compareInitiative)
}

/**
 * Assign position values based on initiative rolls.
 * Higher roll = higher position number = goes first.
 */
export function assignPositions(characters: Character[]): Character[] {
  // Sort by initiative roll (stat INIT_ID) descending
  const sorted = [...characters].sort((a, b) => {
    const rollA = a.stats['INIT_ID']?.roll ?? 0
    const rollB = b.stats['INIT_ID']?.roll ?? 0
    if (rollA !== rollB) return rollB - rollA

    // Tie-breaker: higher modifier goes first
    const modA = a.stats['INIT_ID']?.mod ?? 0
    const modB = b.stats['INIT_ID']?.mod ?? 0
    return modB - modA
  })

  return sorted.map((ch, idx) => ({
    ...ch,
    position: sorted.length - idx,
  }))
}

/**
 * Find the next initiative position after the current one.
 * Handles wrapping around to the next round.
 * Skips dead characters if skipDead is true.
 */
export function peekNextInit(
  characters: Character[],
  currentInit: number,
  skipDead: boolean,
  deathThreshold: number,
): { nextInit: number; roundIncrement: boolean } {
  const sorted = sortByInitiative(characters)
  const activeChars = sorted.filter(ch => {
    if (ch.deleted) return false
    if (skipDead) {
      const hp = getCurrentHP(ch)
      if (hp <= deathThreshold) return false
    }
    return true
  })

  if (activeChars.length === 0) {
    return { nextInit: 0, roundIncrement: false }
  }

  // Find current character's index
  const currentIdx = activeChars.findIndex(ch => {
    const pos = ch.manualPos > 0 ? ch.manualPos : ch.position
    return pos === currentInit
  })

  if (currentIdx === -1 || currentIdx >= activeChars.length - 1) {
    // Wrap to first character, new round
    const firstChar = activeChars[0]
    const pos = firstChar.manualPos > 0 ? firstChar.manualPos : firstChar.position
    return { nextInit: pos, roundIncrement: true }
  }

  const nextChar = activeChars[currentIdx + 1]
  const pos = nextChar.manualPos > 0 ? nextChar.manualPos : nextChar.position
  return { nextInit: pos, roundIncrement: false }
}

/**
 * Check if there are duplicate initiative values that need resolving.
 */
export function findDuplicateInitiatives(characters: Character[]): Map<number, Character[]> {
  const rollMap = new Map<number, Character[]>()

  for (const ch of characters) {
    if (ch.deleted) continue
    const roll = ch.stats['INIT_ID']?.roll ?? 0
    const mod = ch.stats['INIT_ID']?.mod ?? 0
    const total = roll + mod
    const existing = rollMap.get(total) || []
    existing.push(ch)
    rollMap.set(total, existing)
  }

  // Only return groups with duplicates
  const duplicates = new Map<number, Character[]>()
  for (const [key, chars] of rollMap) {
    if (chars.length > 1) {
      duplicates.set(key, chars)
    }
  }

  return duplicates
}

/**
 * Get the character at a specific initiative position.
 */
export function getCharacterAtPosition(characters: Character[], position: number): Character | undefined {
  return characters.find(ch => {
    const pos = ch.manualPos > 0 ? ch.manualPos : ch.position
    return pos === position
  })
}

// ============================================================================
// Start-rounds helper — shared by Toolbar, App menu handler, and any other
// caller that needs to kick off combat.
// ============================================================================

/**
 * Describes what the caller should do to start rounds.
 *
 * If `openDialog` is true the caller should open the initiative roll dialog
 * (manualInitiative mode) rather than dispatching the ready-made actions.
 * In that case `actionsToDispatch` still contains the NPC auto-rolls so the
 * caller can pre-populate the dialog's monster rolls.
 */
export interface StartRoundsResult {
  /** Whether to open the initiative-roll dialog instead of auto-starting. */
  openDialog: boolean
  /**
   * Redux-compatible plain action objects to dispatch.
   * In auto mode: stat-value rolls + position updates + startRounds.
   * In manual mode: NPC stat-value rolls only (dialog handles the rest).
   */
  actionsToDispatch: Array<{
    type: string
    payload: unknown
  }>
}

/**
 * Compute all Redux actions needed to roll initiative and begin combat.
 *
 * Rules:
 *  • rollInitOnStart = false  → skip rolling; just re-assign positions from
 *    whatever rolls are already stored, then start rounds immediately.
 *  • rollInitOnStart = true, manualInitiative = false  → auto-roll every
 *    character, assign positions, start rounds immediately.
 *  • rollInitOnStart = true, manualInitiative = true   → auto-roll monsters
 *    only, signal the caller to open the init-roll dialog (which handles
 *    player entries and then starts rounds itself).
 *
 * This function is framework-agnostic: it returns plain action objects so
 * it can be called from React hooks, plain callbacks, or the menu handler
 * in App.tsx without any code duplication.
 */
export function buildStartRoundsResult(
  characters: Character[],
  statDefs: StatDefinition[],
  settings: Pick<AppSettings, 'rollInitOnStart' | 'manualInitiative'>,
): StartRoundsResult {
  const initStat = statDefs.find(s => s.id === 'INIT_ID')
  const dice  = initStat?.dice  ?? 1
  const faces = initStat?.faces ?? 20

  const active = characters.filter(c => !c.deleted)
  const actions: Array<{ type: string; payload: unknown }> = []

  if (settings.rollInitOnStart) {
    if (!settings.manualInitiative) {
      // Auto-roll every combatant
      active.forEach(ch => {
        const mod  = ch.stats['INIT_ID']?.mod ?? 0
        const roll = makeStatRoll(dice, faces, mod)
        actions.push({
          type: 'characters/setStatValue',
          payload: { charId: ch.id, statId: 'INIT_ID', value: { roll } },
        })
      })
    } else {
      // Manual mode: auto-roll monsters only; players enter their own values.
      active.filter(ch => ch.monster).forEach(ch => {
        const mod  = ch.stats['INIT_ID']?.mod ?? 0
        const roll = makeStatRoll(dice, faces, mod)
        actions.push({
          type: 'characters/setStatValue',
          payload: { charId: ch.id, statId: 'INIT_ID', value: { roll } },
        })
      })
      // Signal the caller to open the dialog — the dialog handles positions
      // and startRounds itself after the player entries are confirmed.
      return { openDialog: true, actionsToDispatch: actions }
    }
  }

  // Build updated character list reflecting the rolls we just queued so that
  // assignPositions sorts correctly even before the actions hit the store.
  const rolledChars = active.map(ch => {
    const rolled = actions.find(
      a => (a.payload as { charId: string }).charId === ch.id,
    )
    if (!rolled) return ch
    const { value } = rolled.payload as { statId: string; charId: string; value: { roll: number } }
    return {
      ...ch,
      stats: {
        ...ch.stats,
        INIT_ID: { ...(ch.stats['INIT_ID'] || { mod: 0, notes: '' }), roll: value.roll },
      },
    }
  })

  const positioned = assignPositions(rolledChars)
  positioned.forEach(ch => {
    actions.push({
      type: 'characters/updateCharacter',
      payload: { id: ch.id, changes: { position: ch.position } },
    })
  })

  const first = positioned[0]
  const startingInit = first ? first.position : 1
  actions.push({
    type: 'initiative/startRounds',
    payload: { startingInit },
  })

  return { openDialog: false, actionsToDispatch: actions }
}
