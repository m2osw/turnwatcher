// Dice rolling utilities
// Mirrors the C++ Character::makeRoll logic

/**
 * Roll a single die with the given number of faces.
 * Returns a value from 1 to faces (inclusive).
 */
export function rollDie(faces: number): number {
  return Math.floor(Math.random() * faces) + 1
}

/**
 * Roll multiple dice and return the total.
 * e.g., rollDice(2, 10) rolls 2d10
 */
export function rollDice(count: number, faces: number): number {
  let total = 0
  for (let i = 0; i < count; i++) {
    total += rollDie(faces)
  }
  return total
}

/**
 * Roll dice with a modifier.
 * e.g., rollWithMod(1, 20, 3) = 1d20+3
 */
export function rollWithMod(count: number, faces: number, modifier: number): number {
  return rollDice(count, faces) + modifier
}

/**
 * Parse a dice string like "1d20+3" and roll it.
 * Returns { total, rolls[], modifier }
 */
export function parseDiceString(diceStr: string): { total: number; rolls: number[]; modifier: number } {
  const match = diceStr.trim().match(/^(\d+)d(\d+)([+-]\d+)?$/)
  if (!match) {
    return { total: 0, rolls: [], modifier: 0 }
  }

  const count = parseInt(match[1], 10)
  const faces = parseInt(match[2], 10)
  const modifier = match[3] ? parseInt(match[3], 10) : 0

  const rolls: number[] = []
  for (let i = 0; i < count; i++) {
    rolls.push(rollDie(faces))
  }

  const total = rolls.reduce((sum, r) => sum + r, 0) + modifier
  return { total, rolls, modifier }
}

/**
 * Make a stat roll for a character.
 * Uses the stat definition's dice/faces and the character's modifier.
 */
export function makeStatRoll(dice: number, faces: number, charModifier: number): number {
  if (dice <= 0 || faces <= 0) return 0
  return rollDice(dice, faces) + charModifier
}
