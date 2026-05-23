import re

import pandas as pd
import streamlit as st
import plotly.express as px
import json

# -------------------------
# 📥 File Handling
# -------------------------
def load_uploaded_file(uploaded_file):
    return pd.read_csv(uploaded_file)


# -------------------------
# 🧹 Data Cleaning
# -------------------------
def cast_columns(df):
    df["Date"] = pd.to_datetime(df["Date"], errors="coerce")
    df["Amount"] = pd.to_numeric(df["Amount"], errors="coerce")

    # drop invalid rows
    df = df.dropna(subset=["Date", "Amount"])
    return df


def process_by_source(df, source):
    if source == "fidelity":
        if "Transaction" in df.columns:
            df = df[df["Transaction"] == "DEBIT"]

        df = df[["Name", "Date", "Amount"]]

    elif source == "discover":
        df = df.rename(columns={
            "Description": "Name",
            "Post Date": "Date",
        })

        df = df[["Name", "Date", "Amount"]]
        df = df[df["Amount"] > 0]

    elif source == "custom":
        required_columns = {"Name", "Date", "Amount"}

        # Category is optional so exclude it
        if (set(df.columns) - {"Category"}) != required_columns:
            st.error(
                """
                Custom CSV files must contain EXACTLY these columns:

                - Name
                - Date
                - Amount
                """
            )
            return None

    elif not source:
        return None

    df["Amount"] = df["Amount"].abs().round(2)
    return df


# -------------------------
# 🏷️ Categorization
# -------------------------
def apply_categories(df, categories):
    df["Name"] = df["Name"].astype(str).str.upper()
    df["Category"] = "Miscellaneous"

    for category, keywords in categories.items():
        # ensure they're all uppercase to allow for case-insensitive matching
        keywords_upper = [k.upper() for k in keywords]

        pattern = "|".join(map(re.escape, keywords_upper))

        mask = df["Name"].str.contains(
            pattern,
            na=False,
            regex=True
        )

        # For all rows where mask == True,
        # set Category column to current category
        df.loc[mask, "Category"] = category

    return df


# -------------------------
# 📊 Aggregation
# -------------------------
def aggregate_data(df):
    df["Month"] = df["Date"].dt.to_period("M").astype(str)

    return (
        df.groupby(["Category", "Month"])["Amount"]
        .sum()
        .reset_index(name="TotalAmount")
        .sort_values("Month")
    )


# -------------------------
# 📈 Visualization
# -------------------------
def plot_data(df):
    df = df.sort_values("Month")

    fig = px.bar(
        df,
        x="Month",
        y="TotalAmount",
        color="Category",
        barmode="stack",
        title="Monthly Spending by Category"
    )

    fig.update_xaxes(type="category")

    st.plotly_chart(fig, width='stretch')


# -------------------------
# 🚀 Main App
# -------------------------
def main():
    st.title("Transaction Analyzer")

    uploaded_file = st.file_uploader(
        "Upload transactions CSV",
        type=["csv"]
    )

    if not uploaded_file:
        st.info("Please upload a CSV file to continue.")
        return

    try:
        df = load_uploaded_file(uploaded_file)
    except Exception as e:
        st.error(f"Failed to read file: {e}")
        return

    source = st.selectbox(
        "Select file type",
        ["", "fidelity", "discover", "custom"]
    )

    # -------------------------
    # Custom Instructions
    # -------------------------
    custom_categories = None
    has_category_column = False

    if source == "custom":
        required_columns = {"Name", "Date", "Amount"}
        missing = required_columns - set(df.columns)
        if missing:
            st.error(
                f"""
                Missing required columns:

                {sorted(missing)}

                Custom CSV files must contain:
                - Name
                - Date
                - Amount
                - Category (optional but required if not providing JSON)
                """
            )
            return

        has_category_column = "Category" in df.columns
        if has_category_column:
            st.success(
                "Detected existing 'Category' column in CSV. "
                "No category JSON file required."
            )
        else:
            st.info(
                """
                No 'Category' column detected.

                Please upload a JSON file for transaction categorization.

                Example:

                {
                    "Food": ["MCDONALDS", "STARBUCKS"],
                    "Travel": ["DELTA", "UBER"]
                }
                """
            )

            uploaded_json = st.file_uploader(
                "Upload category JSON file",
                type=["json"]
            )

            if not uploaded_json:
                return

            try:
                custom_categories = json.load(uploaded_json)
            except Exception as e:
                st.error(f"Invalid JSON file: {e}")
                return

    df = process_by_source(df, source)

    if df is None:
        st.error("Please select a valid file type.")
        return

    df = cast_columns(df)

    # -------------------------
    # Default Categories
    # -------------------------
    categories = {
        "Income": ["PAYROLL", "DIRECT DEP", "VENMO", "REFUND"],
        "Housing": ["RENT", "MORTGAGE", "ELECTRIC"],
        "Transportation & Gas": [
            "SHELL",
            "EXXON",
            "UBER",
            "LYFT",
            "7-ELEVEN",
            "Sam's Club Gas",
            "SAMS FUEL",
            "GAS",
        ],
        "Food & Dining": [
            "STARBUCKS",
            "DOMINOS",
            "DOORDASH",
            "TACO",
            "MCDONALDS",
        ],
        "Grocery": [
            "COSTCO",
            "KROGER",
            "TRADER JOE",
            "WALMART",
            "ALDI",
            "WM SUPERCENTER",
            "BRAVO",
            "WHOLEFDS",
            "Enson Market Inc",
            "PUBLIX",
            "LOTTE PLAZA",
            "SAMS STORE",
        ],
        "Entertainment": ["NETFLIX", "SPOTIFY", "DISNEY"],
        "Health & Medical": ["CVS", "WALGREENS", "KAISER"],
        "Shopping & Personal": ["AMAZON", "BESTBUY", "MACY"],
        "Travel": ["HOTEL", "AIRBNB", "DELTA"],
        "Financial / Investments": ["CREDIT", "IRA", "401K", "LOAN"],
    }

    # Use custom categories if uploaded
    if "Category" not in df.columns:
        if custom_categories:
            categories = custom_categories
        df = apply_categories(df, categories)

    df = aggregate_data(df)

    st.dataframe(df)

    # -------------------------
    # CSV Export
    # -------------------------
    csv_data = df.to_csv(index=False).encode("utf-8")

    st.download_button(
        label="Download Aggregated CSV",
        data=csv_data,
        file_name="aggregated_transactions.csv",
        mime="text/csv",
    )

    plot_data(df)

# ❯ poetry run streamlit run transactions_processor_pipeline.py 
if __name__ == "__main__":
    main()