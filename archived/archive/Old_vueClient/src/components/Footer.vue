<template>
  <footer :class="`${isDark ? 'bg-dark bg-gradient' : 'bg-light bg-gradient-light'}  text-center py-3 mt-5`">
    <div class="container">
      <small>&copy; {{ year }} My Finance App. All rights reserved.</small>
    </div>
  </footer>
</template>

<script setup lang="ts">
import { useTheme } from "../stores/theme-store";
const year = new Date().getFullYear();
const { isDark } = useTheme();
</script>
