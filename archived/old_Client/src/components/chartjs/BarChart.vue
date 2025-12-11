<template>
  <canvas ref="canvasRef"></canvas>
</template>

<script setup lang="ts">
import {
  BarController,
  BarElement,
  CategoryScale,
  Chart,
  Legend,
  LinearScale,
  Title,
  Tooltip,
} from "chart.js";
import ChartDataLabels from "chartjs-plugin-datalabels";
import { computed, onBeforeUnmount, ref, watch } from "vue";
import type { CategorySum } from "../../types/CategorySum";
import { randomHexColor } from "../../utils/utils";

const { categoryTotals } = defineProps<{
  categoryTotals: CategorySum;
}>();

const labels = ref<string[]>([]);
const data = ref<number[]>([]);

const l = computed(() => [...labels.value]);
const d = computed(() => [...data.value]);

const canvasRef = ref<HTMLCanvasElement | null>(null);
let chartInstance: Chart | null = null;

watch(
  () => categoryTotals,
  (newTotals) => {
    // console.log('categoryTotals',categoryTotals);
    labels.value = Object.keys(newTotals);
    data.value = Object.values(newTotals);

    if (canvasRef.value && labels.value && data.value) {
      // Destroy existing chart if present
      if (chartInstance) {
        chartInstance.destroy();
        chartInstance = null;
      }
      chartInstance = new Chart(canvasRef.value, {
        type: "bar",
        data: {
          labels: l.value,
          datasets: [
            {
              label: "",
              data: d.value,
              backgroundColor: l.value.map((_) => randomHexColor()),
            },
          ],
        },
        options: {
          responsive: true,
          plugins: {
            datalabels: {
              color: "#ffffff", // text color of the labels on bars
              anchor: "center", // position relative to bar (start, center, end)
              align: "center", // alignment of the label (start, center, end, top, bottom)
              font: { weight: "bold" },
              formatter: (value) => value.toFixed(2), // format the number
              // offset: 6,
            },
            legend: {
              display: false,
              labels: {
                color: "#ffffff",
              },
            },
            title: {
              display: true,
              text: "Spendings",
              color: "#ffffff",
              font: {
                size: 18,
                weight: "bold",
              },
            },
          },
          scales: {
            x: {
              ticks: {
                color: "#ffffff",
              },
            },
            y: {
              ticks: {
                color: "#ffffff",
              },
            },
          },
        },
      });
    }

    // console.log(l.value);
    // console.log(d.value);
  },
  { immediate: true, deep: true }
);

// Register only what you need
Chart.register(
  BarController,
  BarElement,
  CategoryScale,
  LinearScale,
  Title,
  Tooltip,
  Legend,
  ChartDataLabels
);

onBeforeUnmount(() => {
  chartInstance?.destroy();
});
</script>

<style scoped>
canvas {
  max-width: 600px;
  max-height: 400px;
}
</style>
