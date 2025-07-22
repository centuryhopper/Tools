// src/types/global.d.ts
interface PlaidLinkHandler {
  open: () => void
  exit: (options?: { force: boolean }) => void
}

interface PlaidCreateConfig {
  token: string
  onSuccess: (public_token: string, metadata: object) => void
  onExit?: (error: object | null, metadata: object) => void
  onEvent?: (eventName: string, metadata: object) => void
}

interface Plaid {
  create: (config: PlaidCreateConfig) => PlaidLinkHandler
}

interface Window {
  Plaid: Plaid
}
