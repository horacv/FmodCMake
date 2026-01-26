"""
Raylib Installer for CI/CD
Extracts Raylib installer, copies API files to the project, and cleans up.
"""

import shutil
import sys
import tempfile

from pathlib import Path

# Project directories
PROJECT_ROOT = Path(__file__).parent.parent
RAYLIB_LIB_DIR = PROJECT_ROOT / "libs" / "raylib"

# Platform-specific configuration
PLATFORM_CONFIG = {
    'windows': {
        'lib_subdir': 'win_x64',
        'api_structure': {
            'inc_dir': 'include',
            'lib_dir': 'lib'
        }
    },
    'linux': {
        'lib_subdir': 'linux_x64',
        'api_structure': {
            'inc_dir': 'include',
            'lib_dir': 'lib'
        }
    },
    'mac': {
        'lib_subdir': 'mac',
        'api_structure': {
            'inc_dir': 'include',
            'lib_dir': 'lib'
        }
    }
}


def detect_platform(installer_path):
    """Detect platform from installer filename."""
    name = installer_path.name.lower()
    if 'win' in name and name.endswith('.zip'):
        return 'windows'
    elif 'linux' in name and name.endswith('.tar.gz'):
        return 'linux'
    elif 'macos' in name and name.endswith('.tar.gz'):
        return 'mac'
    else:
        raise ValueError(f"Cannot detect platform from installer: {installer_path.name}")


def extract_installer(installer_path, temp_dir):
    """Extract .zip or .tar.gz archive."""
    print("Extracting archive...")
    shutil.unpack_archive(str(installer_path), temp_dir)
    print("✓ Extracted archive successfully")


def copy_api_files(temp_dir, platform):
    """Copy Raylib libs from temp directory to project."""
    print("\nCopying Raylib files to project...")

    config = PLATFORM_CONFIG[platform]
    api_structure = config['api_structure']

    inc_src_dirs = list(temp_dir.rglob(api_structure['inc_dir']))
    lib_src_dirs = list(temp_dir.rglob(api_structure['lib_dir']))

    # Copy headers
    src = inc_src_dirs[0] if inc_src_dirs else ""
    dst = RAYLIB_LIB_DIR / api_structure['inc_dir']
    if src.exists():
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(src, dst, dirs_exist_ok=True)
        print(f"✓ Copied Raylib headers ({len(list(dst.glob('*')))} files)")
    else:
        print(f"⚠ Warning: Raylib headers not found at {src}")

    # Copy libraries
    src = lib_src_dirs[0] if lib_src_dirs else ""
    dst = RAYLIB_LIB_DIR / api_structure['lib_dir'] / config['lib_subdir']
    if src.exists():
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(src, dst, dirs_exist_ok=True)
        print(f"✓ Copied Raylib libraries ({len(list(dst.glob('*')))} files)")
    else:
        print(f"⚠ Warning: Raylib libraries not found at {src}")


def main():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Usage: python install_raylib.py <installer_file> [--delete-installer]")
        print("Example: python install_raylib.py raylib-5.5_win64_msvc16.zip")
        print("Example: python install_raylib.py raylib-5.5_win64_msvc16.zip --delete-installer")
        print("\nOptions:")
        print("  --delete-installer    Delete the installer file after successful installation")
        sys.exit(1)

    installer_path = Path(sys.argv[1])
    delete_installer = len(sys.argv) == 3 and sys.argv[2] == '--delete-installer'

    # Validate installer exists
    if not installer_path.is_file():
        print(f"Error: Installer not found: {installer_path}")
        sys.exit(1)

    print("\n" + "=" * 60)
    print(f"Raylib Installer")
    print("\n" + "=" * 60)

    # Detect platform
    try:
        platform = detect_platform(installer_path)
        print(f"Installer found: {installer_path.name}")
        print(f"Platform: {platform}")
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)

    # Create a temporary directory
    temp_dir = Path(tempfile.mkdtemp(prefix="temp_raylib_install_"))
    print(f"Temp directory: {temp_dir}")

    try:
        # Extract installer
        extract_installer(installer_path, temp_dir)

        # Copy API files to the project
        copy_api_files(temp_dir, platform)

        print("\n✓ Installation complete!")
        print(f"Raylib files copied to: {RAYLIB_LIB_DIR}")

        # Delete installer if requested
        if delete_installer:
            print("\n[--delete-installer] flag passed. Deleting installer...")
            installer_path.unlink()
            print(f"✓ Deleted {installer_path.name}")

    except Exception as e:
        print(f"\nError: {e}")
        sys.exit(1)
    finally:
        # Cleanup temporary directory
        print("\nCleaning up...")
        shutil.rmtree(temp_dir, ignore_errors=False)
        print("✓ Temporary files removed")


if __name__ == "__main__":
    main()