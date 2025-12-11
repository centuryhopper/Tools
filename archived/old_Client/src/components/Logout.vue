<template></template>

<script lang="ts" setup>
import { onMounted } from "vue";
import { useRouter } from "vue-router";
import { authStore } from "../stores/auth";

onMounted(() => {
  const { logout } = authStore();
  logout();
  const router = useRouter();
  router.push("/");
});
</script>

<style scoped></style>
