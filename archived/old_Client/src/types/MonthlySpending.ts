
/*
{
        "month": "2025-07",
        "categorySum": {
            "total": 6,
            "category": "Transportation"
        }
    },
*/

export type MonthlySpending = {
    month: string;
    categorySum: {
        total: number;
        category: string;
    }
}
