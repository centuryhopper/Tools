#include "../include/img_lib.hpp"
#include <chrono>
#include <filesystem>

// Loop thru all folders starting with group_
// Move first item out of each group_ folder into parent directory and remove
// that group_ folder
void deleteImgGroups(const fs::path &PARENT_PATH)
{
    for (const auto &entry : fs::directory_iterator(PARENT_PATH))
    {
        if (!entry.is_directory())
            continue;

        const fs::path groupDir = entry.path();
        const std::string name = groupDir.filename().string();

        // only process folders starting with "group_"
        if (!name.starts_with("group_"))
            continue;

        // find the first regular file
        for (const auto &item : fs::directory_iterator(groupDir))
        {
            if (!item.is_regular_file())
                continue;

            fs::path src = item.path();
            fs::path dst = PARENT_PATH / src.filename();

            // move first item back into parent directory
            if (!fs::exists(dst))
            {
                fs::rename(src, dst);
            }
            else
            {
                std::println("Skipping {}, destination already exists",
                             src.string());
            }

            break;
        }

        // removes the group folder AND everything remaining inside it
        fs::remove_all(groupDir);
    }
}

std::vector<Match> findImgMatches(const std::vector<ImageInfo> &images,
                                  chr::seconds window)
{
    const int TOLERABLE_PHASH_DISTANCE = 5;

    const auto isSimilar = [](const ImageInfo &img1,
                              const ImageInfo &img2) -> bool
    {
        double distance = comparePHash(img1.phash, img2.phash);
        // std::println(
        //     "{} vs {} -> pHash distance: {}",
        //     img1.path.string(),
        //     img2.path.string(),
        //     distance
        // );

        // union the two image indices
        if (distance <= TOLERABLE_PHASH_DISTANCE)
        {
            // std::println("phash distance between {} and {}: {}",
            // img1.path.filename().string(), img2.path.filename().string(),
            // distance);
            return true;
        }

        return false;
    };

    tbb::concurrent_vector<Match> matches;

    // the uz suffix was added in C++23, it means the same thing as
    // std::size_t{0}, just shorter
    tbb::parallel_for(0uz, images.size(),
                      [&](std::size_t i)
                      {
                          for (auto j = i + 1;
                               j < images.size() &&
                               images[j].timestamp - images[i].timestamp <=
                                   window;
                               ++j)
                          {
                              if (isSimilar(images[i], images[j]))
                                  matches.emplace_back(i, j);
                          }
                      });

    return std::vector<Match>(matches.begin(), matches.end());
}
void groupImages(const fs::path &PARENT_PATH)
{
    auto images = getImages(PARENT_PATH);

    std::println("Number of images: {}", images.size());

    const chr::duration WINDOW = chr::seconds{30};

    /*
        0–2: extremely similar / near-duplicate
        3–5: strong match candidate
        6–10: looser similarity, worth eyeballing
        >10: increasingly likely to be unrelated
    */
    std::unordered_map<int, std::vector<int>> imgGroups;
    imgGroups.reserve(images.size());
    custom_data_structures::UnionFind uf(images.size());

    // O (n * (n-1) / 2) comparisons in the worst case for sequential approach

    auto matches = findImgMatches(images, WINDOW);
    for (const auto &[i, j] : matches)
    {
        uf.unite(i, j);
    }

    for (size_t i = 0; i < images.size(); i++)
    {
        int x = uf.find(i);
        imgGroups[x].push_back(i);
    }

    // Create folders based off of the roots of the union find data structure
    // and move each image their corresponding root folders
    fs::path uniqueFolder = PARENT_PATH / "unique_imgs";

    fs::create_directories(uniqueFolder);

    for (const auto &[root, indices] : imgGroups)
    {
        // fmt::println("root {} -> {}", root, indices);

        if (indices.size() == 1)
        {
            // fmt::println("only one img found");
            fs::path imgPath = images[indices[0]].path;
            fs::rename(imgPath, uniqueFolder / imgPath.filename());
            // fs::copy_file(
            //     imgPath,
            //     uniqueFolder / imgPath.filename(),
            //     fs::copy_options::skip_existing
            // );
            continue;
        }

        fs::path folderName = PARENT_PATH / std::format("group_{}", root);
        fs::create_directories(folderName);

        for (int idx : indices)
        {
            fs::path imgPath = images[idx].path;
            fs::rename(imgPath, folderName / imgPath.filename());
            // fs::copy_file(
            //     imgPath,
            //     folderName / imgPath.filename(),
            //     fs::copy_options::skip_existing
            // );
        }
    }
}

bool isImage(const fs::path &path)
{
    std::string ext = path.extension().string();

    rng::transform(ext, ext.begin(), [](char c) { return std::tolower(c); });

    return ext == ".jpg" || ext == ".jpeg" || ext == ".png";
}

std::optional<Timestamp> getImgCaptureTime(const fs::path &path)
{
    // std::println("calling with '{}'", path.string()); // temporary debug
    std::array<char, 20> date{};
    if (!get_img_timestamp(path.c_str(), date.data(), date.size()))
        return std::nullopt;

    std::istringstream in(date.data());
    chr::local_seconds local; // camera clock reading, no zone
    in >> chr::parse("%Y:%m:%d %H:%M:%S", local);
    if (in.fail())
        return std::nullopt;

    return chr::current_zone()->to_sys(local, chr::choose::earliest);
}

std::vector<ImageInfo> getImages(const fs::path &directory)
{
    std::error_code ec;
    std::println("getting images...");
    std::vector<ImageInfo> images;

    for (fs::directory_iterator it(directory, ec), end; !ec && it != end;
         it.increment(ec))
    {
        auto entry = *it;
        if (!entry.is_regular_file())
            continue;
        if (!isImage(entry.path()))
            continue;

        ImageInfo image{
            {
                entry.path(),
                getImgCaptureTime(entry.path()).value_or(Timestamp{}),
            },
            computePHash(entry.path())};

        images.push_back(std::move(image));
    }

    if (ec)
    {
        std::println(stderr, "img error: {}", ec.message());
    }
    rng::sort(images, [](const ImageInfo &a, const ImageInfo &b)
              { return a.timestamp < b.timestamp; });

    return images;
}
