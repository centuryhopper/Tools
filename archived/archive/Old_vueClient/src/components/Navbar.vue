<template>
  <nav
    :class="`navbar navbar-expand-md ${
      isDark ? 'bg-dark' : ''
    } border-bottom border-secondary fixed-top shadow`"
  >
    <div
      class="container-fluid px-4 py-2 d-flex justify-content-between align-items-center"
    >
      <!-- hamburger - mobile only -->
      <button
        @click="isMenuOpen = true"
        :class="`d-md-none btn btn-link text-${isDark ? 'white' : 'black'} p-0`"
        aria-label="Open menu"
      >
        <Menu :size="28" />
      </button>

      <div
        :class="`position-absolute top-50 start-50 translate-middle text-${
          isDark ? 'white' : 'black'
        } fw-bold text-uppercase text-center d-md-none`"
      >
        <router-link
          :class="`nav-link text-${isDark ? 'white' : 'black'} text-decoration-none`"
          to="/"
          >Personal Spending</router-link
        >
      </div>

      <div
        :class="`d-none d-md-block text-${isDark ? 'white' : ''} fw-bold text-uppercase`"
      >
        <router-link
          :class="`nav-link text-${isDark ? 'white' : ''} text-decoration-none`"
          to="/"
          >Personal Spending</router-link
        >
      </div>

      <!-- desktop nav -->
      <div class="d-none d-md-flex gap-4 fw-medium">
        <span
          :key="link.name"
          class="btn-dark p-3"
          v-for="link in navLinks"
          style="border-radius: 10px"
        >
          <router-link
            :class="`nav-link text-${isDark ? 'white' : ''} text-decoration-none`"
            :to="link.href"
            >{{ link.name }}</router-link
          >
        </span>
      </div>

      <!-- mobile menu overlay -->
      <div v-if="isMenuOpen">
        <div
          class="position-fixed top-0 start-0 w-100 h-100 bg-black bg-opacity-75"
          style="backdropfilter: blur(4px)"
          @click="isMenuOpen = false"
        />
        <aside
          :class="`position-fixed top-0 start-0 ${
            isDark ? 'bg-dark text-white' : 'bg-light text-black'
          } p-4 d-flex flex-column gap-4 h-100 shadow animate-slide-in`"
          style="width: 250px; zindex: 1050; overflowy: auto"
        >
          <button
            @click="isMenuOpen = false"
            :class="`btn btn-link text-${isDark ? 'white' : 'black'} align-self-end p-0`"
            aria-label="Close menu"
          >
            <X :size="28" />
          </button>
          <span
            :key="link.name"
            class="btn-dark p-3"
            v-for="link in navLinks"
            style="border-radius: 10px"
          >
            <router-link
              :class="`nav-link text-${isDark ? 'white' : ''} text-decoration-none fs-5`"
              :to="link.href"
              router-link
              >{{ link.name }}</router-link
            >
          </span>
        </aside>
      </div>
    </div>
  </nav>
</template>

<script setup lang="ts">
import { Menu, X } from "lucide-vue-next";
import { computed, ref } from "vue";
import { authStore } from "../stores/auth";
import { useTheme } from "../stores/theme-store";

const { isDark } = useTheme();

const isMenuOpen = ref(false);

const store = authStore();

const navLinks = computed(() =>
  store.isAuthenticated
    ? [
        { name: "Home", href: "/" },
        { name: "Dashboard", href: "/dashboard" },
        // { name: "Transactions", href: "/transactions" },
        { name: "Budgets", href: "/budgets" },
        { name: "Settings", href: "/settings" },
        { name: "Logout", href: "/logout" },
      ]
    : [
        {
          name: "Login",
          href: "/login",
        },
      ]
);
</script>

<style scoped>
/* * {
    outline: 1px solid red;
  } */

@keyframes slide-in {
  from {
    transform: translateX(-100%);
  }
  to {
    transform: translateX(0);
  }
}
.animate-slide-in {
  animation: slide-in 0.3s ease forwards;
}

.btn-dark {
  position: relative;
  overflow: hidden;
  transition: color 0.3s ease;
  z-index: 0;
}

.btn-dark::before {
  content: "";
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  /*background: linear-gradient(90deg, #06b6d4, #3b82f6, #8b5cf6);/* /* cyan to blue to purple */
  background-image: linear-gradient(90deg, #4b6cb7, #182848);
  /* color: white; */
  border: 10px;
  opacity: 0;
  transition: opacity 0.3s ease;
  z-index: -1;
}

.btn-dark:hover::before {
  opacity: 1;
}

.btn-dark:hover {
  color: white;
}
</style>
