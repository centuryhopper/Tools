import type { RouteRecordRaw } from "vue-router";
import { createRouter, createWebHistory } from "vue-router";
import Budgets from "../components/Budgets.vue";
import Dashboard from "../components/Dashboard.vue";
import HomeView from "../components/HomeView.vue";
import Login from "../components/Login.vue";
import Logout from "../components/Logout.vue";
import Settings from "../components/Settings.vue";

const routes: RouteRecordRaw[] = [
  { path: "/", component: HomeView },
  { path: "/budgets", component: Budgets },
  { path: "/dashboard", component: Dashboard },
  { path: "/settings", component: Settings },
  { path: "/login", component: Login },
  { path: "/logout", component: Logout },
];

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes,
});

export default router;
