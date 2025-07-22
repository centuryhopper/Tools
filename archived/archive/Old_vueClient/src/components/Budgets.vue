<template>
  <div class="container m-5 p-5">
    <h1 class="fw-bold fs-3 mt-5 mb-3 text-center">
      Budget for {{ currentMonthString }}
    </h1>
    <div class="d-flex justify-content-center">
      <div :class="`card ${isDark ? 'bg-dark' : ''} bg-gradient`">
        <div class="card-body">
          <p class="card-text">
            <Budgetsbar
              v-for="value in budgetInfo"
              :key="value.id"
              :category="value.category"
              :spent="value.spent"
              :budget="value.budget"
              @update:budgetLimit="(newLimit) => updatebudgetLimit(value.id, newLimit)"
            />
            <!-- listen for that change -->
          </p>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from "vue";
import { useTheme } from "../stores/theme-store";
import Budgetsbar from "./Budgetsbar.vue";

const { isDark } = useTheme();

function updatebudgetLimit(id: number, newLimit: number) {
  const item = budgetInfo.value.find((b) => b.id === id);
  if (item) item.budget = newLimit;

  console.log("updated budget limit");
  // TODO: make api call here
}

const monthNames = [
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December",
];

const currentMonthString = monthNames[new Date().getMonth()];

const budgetInfo = ref([
  {
    id: 0,
    category: "Groceries",
    budget: 400,
    spent: 255,
  },
  {
    id: 1,
    category: "Rent",
    budget: 1200,
    spent: 1200,
  },
  {
    id: 2,
    category: "Entertainment",
    budget: 400,
    spent: 300,
  },
]);
</script>

<style scoped>
/* * {
  color: white;
} */
</style>
