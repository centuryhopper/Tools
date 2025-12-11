<template>
  <div v-if="authStre.isAuthenticated">
    <div class="container m-5 p-5">
      <h1 class="fw-bold fs-3 text-center m-3">Welcome to My Finance App</h1>
      <!-- <div class="text-center m-3">
        <button class="btn btn-primary mb-4" @click="linkPlaid">
          Connect a bank account
        </button>
      </div> -->

      <div class="d-flex justify-content-evenly">
        <button class="btn btn-primary mb-4" @click="linkPlaid">
          Connect a bank account
        </button>

        <button class="btn btn-primary mb-4" @click="syncTransactions">
          Sync Transactions
        </button>
      </div>

      <div class="row g-4">
        <div class="col-md-6">
          <div
            class="card p-3 shadow-sm w-100 h-100 d-flex flex-column"
            style="max-width: 300px"
          >
            <h5 class="text-center">My accounts</h5>
            <div v-if="!banksLoaded">
              <div class="d-flex justify-content-center">
                <div class="spinner-border" role="status">
                  <span class="visually-hidden">Loading...</span>
                </div>
              </div>
            </div>
            <div v-else-if="banks.length === 0 && banksLoaded">
              <p>No bank accounts were added yet</p>
            </div>
            <div v-else>
              <ul
                class="list-unstyled mb-0 flex-grow-1 overflow-auto align-content-around"
              >
                <li
                  :key="bank.bankinfoid"
                  v-for="bank in banks"
                  class="d-flex justify-content-between mb-1 bank-selection"
                  @click="onBankSelection(bank.bankinfoid, bank.bankname)"
                >
                  <span>{{ bank.bankname }}</span>
                  <strong> ${{ bank.totalbankbalance }}</strong>
                </li>
              </ul>

              <hr class="my-2" />
              <div class="d-flex justify-content-between mt-auto pt-2">
                <span>Total</span>
                <strong>${{ balanceTotal }}</strong>
              </div>
            </div>
          </div>
        </div>

        <div class="col-md-6">
          <div class="card p-3 shadow-sm w-100 h-100" style="max-width: 500px">
            <h5 class="mb-3 fw-semibold text-center">Most recent transactions</h5>
            <div v-if="!transactionsLoaded">
              <div class="d-flex justify-content-center">
                <div class="spinner-border" role="status">
                  <span class="visually-hidden">Loading...</span>
                </div>
              </div>
            </div>
            <div v-else-if="recentTransactions.length === 0 && transactionsLoaded">
              <p>No transactions were saved yet</p>
            </div>
            <div v-else>
              <ul class="list-unstyled">
                <li
                  :key="transaction.id"
                  v-for="transaction in recentTransactions"
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
                    {{ transaction.name }}
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
import axios from "axios";
import { computed, onMounted, ref } from "vue";
import { usePlaid } from "../composables/usePlaid";
import useSweetAlertPopups from "../composables/useSweetAlertPopups";
import { authStore } from "../stores/auth";
import type { BankInfo } from "../types/BankInfo";
import type { Transaction } from "../types/Transactions";
import { API_BASE_URL } from "../utils/utils";

const authStre = authStore();
// console.log(authStre.claims);
const { linkPlaid } = usePlaid();
const { showFeedbackPopup } = useSweetAlertPopups();

const banks = ref<BankInfo[]>([]);
let banksLoaded = ref(false);
const balanceTotal = computed(() =>
  banks.value.reduce((acc, currentAccount) => currentAccount.totalbankbalance + acc, 0)
);

const syncTransactions = async () => {
  // check local storage to see if we already pulled data within 24 hours
  // if we did then show warning popup and terminate early
  // otherwise we can pull data with api call and upsert local storage value
  const lastSaved = localStorage.getItem("lastSaved");
  const selectedBankName = localStorage.getItem("selectedBankName");

  if (!selectedBankName) {
    console.log("no banks found");
    return;
  }

  if (lastSaved) {
    const savedTime = parseInt(lastSaved, 10);
    const now = Date.now();
    const hoursPassed = (now - savedTime) / (1000 * 60 * 60);

    if (hoursPassed >= 24) {
      // make api call
      const response = await axios.get(API_BASE_URL + "api/Plaid/sync-transactions", {
        params: {
          institutionName: selectedBankName,
        },
        headers: {
          Authorization: `Bearer ${authStre.token}`,
        },
      });

      if (response.status === 200) {
        await showFeedbackPopup({
          successMsg: response.data.message,
        });
      } else {
        await showFeedbackPopup({
          success: false,
          failMsg: "No transactions to sync",
        });
      }
      const now = Date.now(); // current time in milliseconds
      localStorage.setItem("lastSaved", now.toString());
    } else {
      console.log(`Only ${hoursPassed.toFixed(2)} hours have passed.`);
    }
  } else {
    // make api call
    const response = await axios.get(API_BASE_URL + "api/Plaid/sync-transactions", {
      params: {
        institutionName: selectedBankName,
      },
      headers: {
        Authorization: `Bearer ${authStre.token}`,
      },
    });

    if (response.status === 200) {
      await showFeedbackPopup({
        successMsg: response.data.message,
      });
    } else {
      await showFeedbackPopup({
        success: false,
        failMsg: "No transactions to sync",
      });
    }

    const now = Date.now(); // current time in milliseconds
    localStorage.setItem("lastSaved", now.toString());
  }
};

const recentTransactions = ref<Transaction[]>([]);
const transactionsLoaded = ref(false);

const onBankSelection = async (bankInfoId: number, bankName: string) => {
  // get transactions depending on the bank selected
  const currentInstitutionId = sessionStorage.getItem("selectedBank") || "-1";

  if (currentInstitutionId === "-1") {
    console.log("no institution found");
    return;
  }

  if (bankInfoId === parseInt(currentInstitutionId)) {
    console.log("already selected");
    return;
  }

  sessionStorage.setItem("selectedBank", bankInfoId.toString());
  sessionStorage.setItem("selectedBankName", bankName);

  transactionsLoaded.value = false;

  const transactionsResponse = await axios.get<Transaction[]>(
    API_BASE_URL + "api/Bank/recent-transactions/" + bankInfoId,
    {
      headers: {
        Authorization: `Bearer ${authStre.token}`,
      },
    }
  );

  recentTransactions.value = transactionsResponse.data;
  transactionsLoaded.value = true;
};

onMounted(async () => {
  if (!authStre.isAuthenticated) {
    return;
  }
  try {
    const bankResponse = await axios.get<BankInfo[]>(
      API_BASE_URL + "api/Bank/get-banks",
      {
        headers: {
          Authorization: `Bearer ${authStre.token}`,
        },
      }
    );
    // console.log(transactionsResponse);
    banks.value = bankResponse.data;
    banksLoaded.value = true;

    if (banks.value.length === 0) {
      transactionsLoaded.value = true;
      return;
    }

    const firstInstitutionId = banks.value[0].bankinfoid;
    sessionStorage.setItem("selectedBank", firstInstitutionId.toString());
    sessionStorage.setItem("selectedBankName", banks.value[0].bankname);

    const transactionsResponse = await axios.get<Transaction[]>(
      API_BASE_URL + "api/Bank/recent-transactions/" + firstInstitutionId,
      {
        headers: {
          Authorization: `Bearer ${authStre.token}`,
        },
      }
    );

    recentTransactions.value = transactionsResponse.data;
    transactionsLoaded.value = true;

    // console.log(recentTransactions);
    // console.log(transactionsLoaded);
  } catch (e) {
    console.error("Error fetching banks:", e);
    await showFeedbackPopup({
      success: false,
      failMsg: "Please try again later.",
    });
  }
});

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
.bank-selection {
  cursor: pointer;
}
.bank-selection:hover {
  background-image: linear-gradient(135deg, #6e8efb, #a777e3);
}
</style>
