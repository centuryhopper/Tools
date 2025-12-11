export type Transaction = {
  id: number;
  userId: number;
  transactionId: string;
  name: string;
  category?: string;
  note?: string;
  amount: number;
  date?: string;
  environmentType: string;
};
