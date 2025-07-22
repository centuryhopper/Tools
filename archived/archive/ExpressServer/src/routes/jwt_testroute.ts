import { Router } from 'express'
import { authenticateToken } from '../middlewares/auth'

const router = Router()

router.get('/public', (_req, res) => {
  res.json({ message: 'This is a public route' })
})

router.get('/protected', authenticateToken, (req, res) => {
  res.json({ message: 'This is a protected route', user: (req as any).user })
})

export default router
