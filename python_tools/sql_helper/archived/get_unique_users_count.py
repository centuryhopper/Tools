import pandas as pd

drm_df = pd.read_csv("./drm_users.csv", usecols=["DRM_UserID", 'DRM_FirstName', 'DRM_LastName']).rename(columns={"DRM_UserID": "user_id"})

# Remove rows where DRM_FirstName or DRM_LastName is empty
drm_df = drm_df[(drm_df["DRM_FirstName"].str.len() > 0) & (drm_df["DRM_LastName"].str.len() > 0)]

# Remove rows where DRM_FirstName contains "EXTERNAL"
drm_df = drm_df[~drm_df["DRM_FirstName"].str.contains("EXTERNAL")]

drm_df = drm_df[~drm_df["DRM_FirstName"].str.contains("bo") & ~drm_df["DRM_LastName"].str.contains("viner")]

drm_df["Name"] = drm_df["DRM_FirstName"].fillna("").str.cat(drm_df["DRM_LastName"].fillna(""), sep=" ")


frrp_df = pd.read_csv("./frrp_users.csv", usecols=["Id", "FirstName", "LastName"]).rename(columns={"Id": "user_id"})

frrp_df["Name"] = frrp_df["FirstName"].fillna("").str.cat(frrp_df["LastName"].fillna(""), sep=" ")

# frrp_df = frrp_df[~frrp_df["Name"].str.contains("Test User")]

combined = pd.concat([
    drm_df,
    frrp_df
])
combined = combined[["user_id", "Name"]]
combined['Name'] = combined['Name'].str.lower()
# dupes = combined[combined.duplicated("Name", keep="first")]
# print(dupes)
# combined_sub = combined[combined["Name"].str.contains("cailin")]
# combined_sub
# print(drm_df.dtypes)
combined = combined.drop_duplicates(subset="Name", keep="first")
# combined

# should print nothing at this point
# dupes = combined[combined.duplicated("Name", keep="first")]
# dupes