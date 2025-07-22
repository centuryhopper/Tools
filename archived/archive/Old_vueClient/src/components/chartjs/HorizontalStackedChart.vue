<script setup lang="ts">
import { Bar } from "vue-chartjs";
import {
  Chart as ChartJS,
  BarElement,
  CategoryScale,
  LinearScale,
  Title,
  Tooltip,
  Legend,
} from "chart.js";

ChartJS.register(BarElement, CategoryScale, LinearScale, Title, Tooltip, Legend);

const chartData = {
  labels: ["January", "February", "March"],
  datasets: [
    {
      label: "Groceries",
      data: [200, 150, 100],
      backgroundColor: "#22c55e",
    },
    {
      label: "Transport",
      data: [100, 120, 130],
      backgroundColor: "#3b82f6",
    },
    {
      label: "Utilities",
      data: [50, 60, 90],
      backgroundColor: "#f59e0b",
    },
  ],
};

const chartOptions = {
  responsive: true,
  indexAxis: "y", // 🔁 Makes it horizontal
  plugins: {
    legend: {
      position: "top" as const,
    },
    title: {
      display: true,
      text: "Monthly Total",
      color: "#ffffff",
    },
  },
  scales: {
    x: {
      stacked: true,
    },
    y: {
      stacked: true,
    },
  },
};
</script>

<template>
  <Bar :data="chartData" :options="chartOptions" />
</template>
