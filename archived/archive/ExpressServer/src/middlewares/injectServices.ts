// src/middlewares/injectServices.ts
import { NextFunction, Request, Response } from "express";
import { AccountService } from "../services/AccountService";
import { PlaidItemService } from "../services/PlaidItemService";

export function injectScopedServices(
  req: Request,
  res: Response,
  next: NextFunction
)
{
  req.accountService = new AccountService();
  req.plaidItemService = new PlaidItemService();
  next();
}
