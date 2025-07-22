// src/db.ts
import dotenv from "dotenv";
import { Pool } from "pg";

dotenv.config();

export const db = new Pool({
  connectionString: process.env.BudgetDB_CONN
});

