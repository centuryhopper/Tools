// src/logger.ts
import { db } from './db'

type LogLevel = 'info' | 'warn' | 'error'

export async function logToDb(level: LogLevel, message: string) {
  try {
    const today = new Date().toISOString().split('T')[0] // YYYY-MM-DD
    await db.query(
      'INSERT INTO logs (date_logged, level, message) VALUES ($1, $2, $3)',
      [today, level, message.slice(0, 255)]
    )
  } catch (err) {
    console.error('Failed to write log to DB:', err)
  }
}
