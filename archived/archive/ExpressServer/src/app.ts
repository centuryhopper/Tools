import express from "express";
import path from "path";
import rateLimiter from "./middlewares/rateLimit";
import accountRouter from "./routes/account_routes";
import plaidRouter from "./routes/plaid_routes";
import { injectScopedServices } from "./middlewares/injectServices";

const app = express();

app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(rateLimiter);

// app.use((req, res, next) => {
//   console.log(`Incoming request: ${req.method} ${req.url}`);
//   next();
// });

// DI containers (singleton)


// DI containers (scoped)
app.use(injectScopedServices);

app.use("/api/account", accountRouter);
app.use("/api/plaid", plaidRouter);

// Serve static files from Vue build
app.use(express.static(path.join(__dirname, "../client_output")));

// Catch-all route to index.html (for Vue router)
// must use this regex syntax for express v5+
app.get(/(.*)/, (req, res) => {
  res.sendFile(path.join(__dirname, "../client_output/index.html"));
});

export default app
