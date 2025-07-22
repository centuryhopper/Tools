<template>
  <div class="container m-5 p-5">
    <h1 class="fw-bold fs-3 mt-5 mb-3 text-center">Settings</h1>
    <div class="d-flex justify-content-center">
      <div :class="`card ${isDark ? 'bg-dark' : ''} bg-gradient`">
        <div class="card-body">
          <div class="card-text">
            <ul class="list-group" style="width: 15rem">
              <li
                v-for="sel in selections"
                :key="sel.name"
                :class="`list-group-item custom-setting-item d-flex justify-content-between align-items-center ${isDark ? 'bg-dark' : ''}`"
                
                @click="sel.callback"
              >
                {{ sel.name }}
                <span class="arrow">&rarr;</span>
              </li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import useSweetAlertPopups from "../composables/useSweetAlertPopups";
import { useTheme } from "../stores/theme-store";
const { confirmationPopup } = useSweetAlertPopups();

const { isDark, toggleTheme } = useTheme();
const selections = [
  {
    name: "App Appearance",
    callback: () => {
      toggleTheme();
      console.log("toggle app appearance!");
    },
  },
  {
    name: "Toggle Two-Factor Authentication",
    callback: toggle2FA,
  },
];

async function toggle2FA() {
  await confirmationPopup(
    "Notice",
    "2FA is currently disabled. Would you like to enable it?",
    async () => {
      await new Promise((resolve) => setTimeout(resolve, 1));
      console.log("enabled 2fa!");
    },
    async () => {
      await new Promise((resolve) => setTimeout(resolve, 1));
      console.log("cancelled enabling 2fa!");
    }
  );
}
</script>

<style scoped>
/* h1 {
  color: white;
} */

.custom-setting-item .arrow {
  transition: transform 0.2s ease;
}

.custom-setting-item:hover .arrow {
  transform: translateX(4px);
}

/* Add this to Settings.css */
.list-group-item.custom-setting-item {
  /* background-color: #000 !important; */
  /* color: white; */
  cursor: pointer;
  transition: background-image 0.3s ease, color 0.3s ease;
}

.list-group-item.custom-setting-item:hover {
  background-image: linear-gradient(90deg, #4b6cb7, #182848) !important;
  /* background-image: linear-gradient(90deg, #06b6d4, #3b82f6, #8b5cf6) !important; */

  /* color: white; */
}
</style>
