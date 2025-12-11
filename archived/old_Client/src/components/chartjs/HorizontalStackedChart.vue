<script setup lang="ts">
import {
  BarElement,
  CategoryScale,
  Chart,
  Chart as ChartJS,
  Legend,
  LinearScale,
  Title,
  Tooltip,
} from "chart.js";
import { onBeforeUnmount, ref, watch } from "vue";
import type { MonthlySpending } from "../../types/MonthlySpending";
import { randomHexColor } from "../../utils/utils";

const { monthlySpending } = defineProps<{
  monthlySpending: MonthlySpending[];
}>();

ChartJS.register(BarElement, CategoryScale, LinearScale, Title, Tooltip, Legend);

const monthLabels = ref<string[]>([]);
const categories = ref<string[]>([]);

const canvasRef = ref<HTMLCanvasElement | null>(null);
let chartInstance: Chart | null = null;

watch(
  () => monthlySpending,
  (newMonthlySpending) => {
    // console.log('newMonthlySpending', newMonthlySpending);
    monthLabels.value = [
      ...new Set(newMonthlySpending.map((sp) => sp.month)),
    ].sort((a, b) => b.localeCompare(a));
    categories.value = [
      ...new Set(newMonthlySpending.map((sp) => sp.categorySum.category)),
    ];

    // Build dataset per category
    const datasets = categories.value.map((category) => {
      const categoryTotal = monthLabels.value.map((month) => {
        const item = monthlySpending.find(
          (ms) => ms.month === month && ms.categorySum.category === category
        );
        return item ? item.categorySum.total : 0;
      });

      return {
        label: category,
        data: categoryTotal,
        backgroundColor: randomHexColor(),
      };
    });

    if (canvasRef.value && monthLabels.value && categories.value) {
      // Destroy existing chart if present
      if (chartInstance) {
        chartInstance.destroy();
        chartInstance = null;
      }

      const maxSpendingVal = Math.max(
        ...monthlySpending.map((obj) => obj.categorySum.total)
      );

      // console.log("datasets", datasets);

      chartInstance = new Chart(canvasRef.value, {
        type: "bar",
        data: {
          labels: monthLabels.value,
          datasets,
        },
        options: {
          responsive: true,
          indexAxis: "y",
          plugins: {
            datalabels: {
              color: "#ffffff", // text color of the labels on bars
              anchor: "end", // position relative to bar (start, center, end)
              align: "end", // alignment of the label (start, center, end, top, bottom)
              clamp: true,
              font: { weight: "bold" },
              formatter: (value) => (value === 0 ? "" : value.toFixed(2)), // format the number
              // offset: 6,
            },
            legend: {
              position: "top",
              labels: {
                color: "#ffffff",
              },
            },
            title: {
              display: true,
              text: "Monthly Spending by Category",
              color: "#ffffff",
              font: {
                size: 18,
                weight: "bold",
              },
            },
          },
          scales: {
            x: {
              min: 0, // Set your desired minimum
              max: maxSpendingVal + 50, // Set your desired maximum
              stacked: true,
              ticks: {
                color: "#ffffff",
              },
            },
            y: {
              stacked: true,
              ticks: {
                color: "#ffffff",
              },
            },
          },
        },
      });
    }
  },
  { immediate: true, deep: true }
);

onBeforeUnmount(() => {
  chartInstance?.destroy();
});

// const chartData = ref({
//   labels: ["January", "February", "March"],
//   datasets: [
//     {
//       label: "Groceries",
//       data: [200, 150, 100],
//       backgroundColor: "#22c55e",
//     },
//     {
//       label: "Transport",
//       data: [100, 120, 130],
//       backgroundColor: "#3b82f6",
//     },
//     {
//       label: "Utilities",
//       data: [50, 60, 90],
//       backgroundColor: "#f59e0b",
//     },
//   ],
// });
</script>

<template>
  <canvas ref="canvasRef"></canvas>
</template>
