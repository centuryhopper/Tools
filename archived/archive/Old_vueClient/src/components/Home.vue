<template>
  <div v-if="authStre.isAuthenticated">
    <div class="container m-5 p-5">
      <h1 class="fw-bold fs-3 text-center m-3">Welcome to My Finance App</h1>
      <div class="text-center m-3">
        <button class="btn btn-primary mb-4" @click="linkPlaid">
          Connect a bank account
        </button>
      </div>

      <div class="row g-4">
        <div class="col-md-6">
          <div
            class="card p-3 shadow-sm w-100 h-100 d-flex flex-column"
            style="max-width: 300px"
          >
            <h5 class="text-center">My accounts</h5>

            <ul class="list-unstyled mb-0 flex-grow-1 overflow-auto align-content-around">
              <li
                :key="bank.id"
                v-for="bank in bankAccounts"
                class="d-flex justify-content-between mb-1"
              >
                <span>{{ bank.name }}</span>
                <strong> ${{ bank.totalBalance }}</strong>
              </li>
            </ul>

            <hr class="my-2" />
            <div class="d-flex justify-content-between mt-auto pt-2">
              <span>Total</span>
              <strong>${{ balanceTotal }}</strong>
            </div>
          </div>
        </div>

        <div class="col-md-6">
          <div class="card p-3 shadow-sm w-100 h-100" style="max-width: 500px">
            <h5 class="mb-3 fw-semibold text-center">Upcoming transactions</h5>
            <ul class="list-unstyled">
              <li
                :key="transaction.id"
                v-for="transaction in fiveMostRecentTransactions"
                class="d-flex align-items-center mb-2 gap-2"
              >
                <div class="text-nowrap text-center" style="width: 90px">
                  {{ transaction.date }}
                </div>
                <div
                  :class="`fw-semibold  ${
                    transaction.amount < 0
                      ? 'badge bg-danger-subtle text-danger'
                      : 'badge bg-success-subtle text-success'
                  } rounded-pill text-center`"
                  style="width: 90px"
                >
                  {{ transaction.amount > 0 ? "+" : "-" }}${{
                    Math.abs(transaction.amount)
                  }}
                </div>
                <div class="text-center flex-grow-1 text-center">
                  {{ transaction.merchant }}
                </div>
                <span
                  class="badge bg-success-subtle text-success rounded-pill text-center"
                >
                  {{ transaction.category }}
                </span>
              </li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  </div>
  <div v-else>
    <div class="container m-5 p-5">
      <h1 className="fw-bold fs-3 text-center">Please Sign in to continue</h1>
      <!-- <button class="btn btn-success m-3" @click="bankLink('test', true)">
        success button
      </button>

      <button class="btn btn-danger m-3" @click="bankLink('test', false)">
        error button
      </button> -->
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed, ref } from "vue";
import { usePlaid } from "../composables/usePlaid";
import useSweetAlertPopups from "../composables/useSweetAlertPopups";
import { authStore } from "../stores/auth";

const authStre = authStore();
// console.log(authStre.claims);
const { linkPlaid } = usePlaid();
const { showFeedbackPopup } = useSweetAlertPopups();

const bankAccounts = ref([
  {
    id: 0,
    name: "Truist",
    totalBalance: 3000,
  },
  {
    id: 1,
    name: "CIT Savings",
    totalBalance: 5000,
  },
]);

const fiveMostRecentTransactions = ref([
  {
    id: 5,
    date: "2023-11-16",
    amount: -120,
    merchant: "Spotify",
    category: "Entertainment",
  },
  {
    id: 6,
    date: "2023-11-17",
    amount: 520,
    merchant: "Upwork",
    category: "Freelance",
  },
  {
    id: 7,
    date: "2023-11-17",
    amount: -65,
    merchant: "Netflix",
    category: "Entertainment",
  },
  {
    id: 8,
    date: "2023-11-18",
    amount: 3500,
    merchant: "Company Co.",
    category: "Bonus",
  },
  {
    id: 9,
    date: "2023-11-18",
    amount: -45,
    merchant: "Apple",
    category: "App Store",
  },
]);

const balanceTotal = computed(() =>
  bankAccounts.value.reduce((acc, currentAccount) => currentAccount.totalBalance + acc, 0)
);

const bankLink = async (bankAccountName: string) => {
  await showFeedbackPopup(
    true,
    `Link to ${bankAccountName} successful!`,
    `Link to ${bankAccountName} unsuccessful :/`
  );
};

// onMounted(() => {
//   console.log('home mounted');
// })

/*
transaction model:
  {
      date: ...,
      amount: ...,
      merchant: ...,
      category: ...,
  }
*/
</script>

<style scoped>
/* * {
  color: white;
} */

/* .card {
  background-color: #2b2d30;
} */
</style>
