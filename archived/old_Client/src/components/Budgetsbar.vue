<template>
  <div
    :class="`m-2 p-3 rounded shadow-sm ${isDark ? 'bg-dark' : ''}`"
    style="width: 30rem"
  >
    <div class="d-flex justify-content-between align-items-center mb-1">
      <span class="fw-semibold">{{ category }}</span>
      <span class="fw-bold">Total spent: ${{ spent.toFixed(2) }}</span>
    </div>
    <div class="progress" style="height: 8px">
      <div
        class="progress-bar bg-primary"
        role="progressbar"
        :style="{ width: progress + '%' }"
        :aria-valuenow="spent"
        aria-valuemin="0"
        :aria-valuemax="budget"
      ></div>
    </div>
    <div class="d-flex justify-content-between align-items-center mb-1">
      <span class="small">$0</span>
      <span v-if="budgetLimitClicked" class="text-end m-1">
        <input
          class="w-50"
          type="text"
          :value="`$${budget.toFixed(2)}`"
          @focusout="updateBudgetLimit"
        />
      </span>
      <span
        v-else
        :class="`text-${budgetThresholdColor} small text-end m-1`"
        @click="toggleBudgetLimit"
        >Limit: ${{ budget.toFixed(2) }}</span
      >
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed, ref } from "vue";
import { useTheme } from "../stores/theme-store";
import type { BudgetBarProps } from "../types/BudgetTypes";

const { category, spent, budget, id } = defineProps<BudgetBarProps>();
const progress = computed(() => Math.min((spent / budget) * 100, 100));
const budgetThresholdColor = computed(() => (spent <= budget ? "" : "danger"));
const budgetLimitClicked = ref(false);
const { isDark } = useTheme();

const emit = defineEmits<{
  (e: "update:budgetLimit", id: number, newBudgetLimit: number): void;
}>();

const toggleBudgetLimit = () => {
  budgetLimitClicked.value = !budgetLimitClicked.value;
};

const updateBudgetLimit = (event: FocusEvent) => {
  const input = event.target as HTMLInputElement;
  const parsed = parseFloat(input.value.replace(/[^0-9.]/g, ""));
  const newBudgetLimit = isNaN(parsed) ? budget : parsed;
  // tell the parent that the value changed!
  emit("update:budgetLimit", id, newBudgetLimit);
  toggleBudgetLimit();
};
</script>
