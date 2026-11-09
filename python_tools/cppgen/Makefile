# Usage:
#   make release                 # patch bump, build, check, publish
#   make release BUMP=minor      # or BUMP=major
#   make build                   # just clean + build
#
# Token: set UV_PUBLISH_TOKEN in your environment (uv reads it automatically),
# or pass it on the command line:  make publish TOKEN=pypi-XXXXXXXX

BUMP  ?= patch
TOKEN ?=

.PHONY: release bump clean build check publish

release: bump clean build check publish

# 1. Bump the version (PyPI won't accept the same version twice)
bump:
	uv version --bump $(BUMP)

# 2. Clear out old builds
clean:
	rm -rf dist/

# 3. Build the sdist + wheel
build: clean
	uv build

# 4. Check the templates made it into the wheel (fails if none found)
check:
	unzip -l dist/*.whl | grep templates

# 5. Publish
publish:
	uv publish $(if $(TOKEN),--token $(TOKEN))
