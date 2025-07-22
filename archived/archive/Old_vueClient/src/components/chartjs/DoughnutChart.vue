<!-- components/DoughnutChart.vue -->
<script setup lang="ts">
import { Doughnut } from "vue-chartjs";
import { Chart as ChartJS, Tooltip, Legend, ArcElement } from "chart.js";
import ChartDataLabels from "chartjs-plugin-datalabels";

ChartJS.register(ArcElement, Tooltip, Legend, ChartDataLabels);

const chartData = {
  // labels: ["Apples", "Bananas", "Cherries"],
  datasets: [
    {
      data: [10, 20, 30],
      backgroundColor: ["#f43f5e", "#facc15", "#34d399"],
    },
  ],
};

const chartOptions = {
  responsive: true,
  plugins: {
    title: {
      display: true,
      color: "#ffffff",
      text: "Spending Distribution",
      font: {
        size: 18,
        weight: "bold",
      },
    },
    datalabels: {
      color: "#fff",
      font: {
        weight: "bold" as const,
      },
      formatter: (value: number, context: any) => {
        const data = context.chart.data.datasets[0].data;
        const total = data.reduce((sum: number, val: number) => sum + val, 0);
        return `${((value / total) * 100).toFixed(1)}%`;
      },
    },
    legend: {
      position: "bottom",
    },
  },
  cutout: "60%",
};

// export default {
//   name: 'DoughnutChart'
// }
</script>

<template>
  <!-- <div style="width: 25rem; height: 25rem"> -->
  <Doughnut :data="chartData" :options="chartOptions" />
  <!-- </div> -->
</template>
