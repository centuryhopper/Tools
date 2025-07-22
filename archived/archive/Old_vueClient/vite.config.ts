import vue from '@vitejs/plugin-vue'
import { defineConfig } from 'vite'

// Vuetify uses SASS, so add sass plugin if needed
import vuetify from 'vite-plugin-vuetify'

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    vue(),
    vuetify({ autoImport: true }), // Vuetify plugin for Vite
  ],
})
