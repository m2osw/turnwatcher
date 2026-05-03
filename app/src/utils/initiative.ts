// Initiative sorting and management utilities
// Mirrors C++ Initiative::InitiativeManager logic

import { Character, Status } from '../types'
import { getCurrentHP } from './health'

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
