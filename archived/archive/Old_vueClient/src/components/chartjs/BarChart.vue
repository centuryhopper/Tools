<template>
  <canvas ref="canvasRef"></canvas>
</template>

<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount } from 'vue'
import {
  Chart,
  BarController,
  BarElement,
  CategoryScale,
  LinearScale,
  Title,
  Tooltip,
  Legend
} from 'chart.js'

// Register only what you need
Chart.register(BarController, BarElement, CategoryScale, LinearScale, Title, Tooltip, Legend)

const canvasRef = ref<HTMLCanvasElement | null>(null)
let chartInstance: Chart | null = null

onMounted(() => {
  if (canvasRef.value) {
    chartInstance = new Chart(canvasRef.value, {
      type: 'bar',
      data: {
        labels: ['Red', 'Blue', 'Yellow'],
        datasets: [{
          label: 'Votes',
          data: [12, 19, 3],
          backgroundColor: ['#f87171', '#60a5fa', '#facc15']
        }]
      },
      options: {
        responsive: true,
        plugins: {
          legend: {
            labels: {
              color: '#ffffff'
            }
          },
          title: {
            display: true,
            text: 'Color Popularity',
            color: '#ffffff',
            font: {
              size: 18,
              weight: 'bold'
            }
          }
        },
        scales: {
          x: {
            ticks: {
              color: '#ffffff'
            }
          },
          y: {
            ticks: {
              color: '#ffffff'
            }
          }
        }
      }
    })
  }
})

onBeforeUnmount(() => {
  chartInstance?.destroy()
})
</script>

<style scoped>
canvas {
  max-width: 600px;
  max-height: 400px;
}
</style>
