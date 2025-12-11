<template>
  <div v-if="didSelectBank">
    <div class="container m-5 p-5">
      <h1 class="fw-bold fs-3 mt-5 mb-3 text-center">Dashboard</h1>
      <div class="row m-3">
        <div class="col-lg-3"></div>
        <div class="col-lg-6">
          <DoughnutChart :categoryTotals="readonlyCategoryTotals" />
        </div>
        <div class="col-lg-3"></div>
      </div>

      <div class="row m-3">
        <div class="col-lg-3"></div>
        <div class="col-lg-6">
          <BarChart :categoryTotals="readonlyCategoryTotals" />
        </div>
        <div class="col-lg-3"></div>
      </div>

      <div class="row">
        <div class="col-lg-3"></div>
        <div class="col-lg-6">
          <HorizontalStackedChart :monthlySpending="readonlyMonthlySpending" />
        </div>
        <div class="col-lg-3"></div>
      </div>

      <div class="m-3">
        <TransactionsGrid :transactions="transactions" />
      </div>
    </div>
  </div>
  <div v-else>
    <h1 class="text-center">
      Please select a bank from the home page to visualize its spendings.
    </h1>
  </div>
</template>

<script setup lang="ts">
import axios from "axios";
import { computed, onMounted, ref } from "vue";
import { authStore } from "../stores/auth";
import type { CategorySum } from "../types/CategorySum";
import type { MonthlySpending } from "../types/MonthlySpending";
import type { Transaction } from "../types/Transactions";
import { API_BASE_URL } from "../utils/utils";
import BarChart from "./chartjs/BarChart.vue";
import DoughnutChart from "./chartjs/DoughnutChart.vue";
import HorizontalStackedChart from "./chartjs/HorizontalStackedChart.vue";
import TransactionsGrid from "./TransactionsGrid.vue";

const selectedBank = sessionStorage.getItem("selectedBank");
const selectedBankName = sessionStorage.getItem("selectedBankName");
const didSelectBank = ref(!!selectedBank);
const authStre = authStore();
const transactions = ref<Transaction[]>([]);

const categoryTotals = ref<CategorySum>({});
const readonlyCategoryTotals = computed(() => categoryTotals.value ?? {});

const monthlySpending = ref<MonthlySpending[]>([]);
const readonlyMonthlySpending = computed(() => monthlySpending.value ?? []);
// Make sure the numbering isn't off for the horizontal stacked chart
onMounted(async () => {
  if (!didSelectBank.value || !selectedBankName) {
    return;
  }
  const response = await axios.get<Transaction[]>(
    API_BASE_URL + "api/Dashboard/transactions/" + parseInt(selectedBank!),
    {
      headers: {
        Authorization: `Bearer ${authStre.token}`,
      },
    }
  );

  transactions.value = response.data;

  // console.log(transactions.value.map((x) => ({ ...x })));

  // Get all its unique categories and use an obj to store them as keys and values will be the sum of all the amounts spent in those categories:
  const uniqueCategories = [
    ...new Set(transactions.value.map((t) => t.category).filter((c) => !!c)),
  ];

  // console.log("uniqueCategories", uniqueCategories);

  const tempTotals: CategorySum = {};

  for (const category of uniqueCategories) {
    let sumTotal = transactions.value.reduce((acc, obj) => {
      if (selectedBankName.toLowerCase().includes("cit bank")) {
        obj.amount = obj.amount > 0 ? -obj.amount : Math.abs(obj.amount);
      }
      return obj.category === category ? acc + obj.amount : acc;
    }, 0);

    // console.log(category, sumTotal);

    sumTotal = Math.max(0, sumTotal);

    if (sumTotal > 0) {
      tempTotals[category!] = Number(sumTotal.toFixed(2));
    }
    categoryTotals.value = tempTotals;
  }

  // console.log(categoryTotals.value);

  const monthlySpendingResponse = await axios.get<MonthlySpending[]>(
    API_BASE_URL + "api/Dashboard/monthlySpendings/" + parseInt(selectedBank!),
    {
      headers: {
        Authorization: `Bearer ${authStre.token}`,
      },
    }
  );

  monthlySpending.value = monthlySpendingResponse.data.filter(
    (sp) => sp.categorySum.total > 0
  );

  // console.log(monthlySpending.value.map((x) => ({ ...x })));
});
</script>

<style scoped>
/* * {
  color: white;
} */
</style>
