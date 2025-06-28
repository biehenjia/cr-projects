import pefile
from pathlib import Path

# Find any pycr*.pyd under build (or build/Debug, etc.)
pyd = next(Path("build").rglob("pycr*.pyd"), None)
if pyd is None:
    raise FileNotFoundError("Could not find pycr*.pyd under build/")
print("Inspecting:", pyd)

pe = pefile.PE(str(pyd))
deps = [entry.dll.decode("utf-8") for entry in pe.DIRECTORY_ENTRY_IMPORT]
print("Dependencies:")
for d in deps:
    print("  ", d)