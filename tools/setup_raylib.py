"""
Raylib Complete Setup
Downloads, installs, and configures Raylib in one command.
"""

import sys
import subprocess
from pathlib import Path

GET_RAYLIB_SCRIPT = 'get_raylib.py'
INSTALL_RAYLIB_SCRIPT = 'install_raylib.py'

def run_command(script_name, args):
    """Run a Python script with arguments and capture output."""
    python_exe = sys.executable
    script_path = Path(__file__).parent / script_name

    cmd = [python_exe, str(script_path)] + args

    result = subprocess.run(
        cmd,
        capture_output=False,  # Show output in real-time
        text=True
    )

    if result.returncode != 0:
        raise RuntimeError(f"{script_name} failed with exit code {result.returncode}")


def main():
    # Parse arguments
    if len(sys.argv) < 3:
        print("Raylib Complete Setup")
        print("=" * 50)
        print("\nUsage: python setup_raylib.py <platform> <raylib_version> [--delete-installer]")
        print("\nPlatforms: mac, linux, windows")
        print("\nOptions:")
        print("  --delete-installer    Delete the installer file after installation (default: keep)")
        print("\nExample:")
        print("  python setup_raylib.py windows 5.5")
        print("  python setup_raylib.py windows 5.5 --delete-installer")
        print("\nThis script will:")
        print("  1. Download the raylib archive")
        print("  2. Extract and copy API files to libs/raylib/")
        print("  3. Clean up temporary files")
        sys.exit(1)

    platform = sys.argv[1]
    raylib_version = sys.argv[2]
    delete_installer = len(sys.argv) > 3 and sys.argv[3] == '--delete-installer'

    print("=" * 60)
    print("Raylib Complete Setup")
    print("=" * 60)
    print(f"Platform: {platform}")
    print(f"Version: {raylib_version}")
    print(f"Delete installer: {'Yes' if delete_installer else 'No'}")
    print("=" * 60)

    try:
        # Step 1: Download the installer
        print("\n[Step 1/2] Downloading raylib installer...")
        print("-" * 60)
        run_command(GET_RAYLIB_SCRIPT, [platform, raylib_version])

        # Step 2: Install and extract API files
        print("\n[Step 2/2] Installing raylib files...")
        print("-" * 60)

        # Determine installer filename
        installer_patterns = {
            'mac': f'raylib-{raylib_version}_macos.tar.gz',
            'linux': f'raylib-{raylib_version}_linux_amd64.tar.gz',
            'windows': f'raylib-{raylib_version}_win64_msvc16.zip'
        }

        installer_name = installer_patterns.get(platform)
        if not installer_name:
            raise ValueError(f"Unknown platform: {platform}")

        # Run the installation script with --delete-installer flag if requested
        install_args = [installer_name]
        if delete_installer:
            install_args.append('--delete-installer')

        run_command(INSTALL_RAYLIB_SCRIPT, install_args)

        # Success message
        print("\n" + "=" * 60)
        print("✓ Raylib Setup Complete!")
        print("=" * 60)
        print(f"API files installed to: libs/raylib/")
        if delete_installer:
            print(f"Installer cleaned up: {installer_name}")
        else:
            print(f"Installer preserved: {installer_name}")
        print("\nYou can now build your project with Raylib")

    except Exception as e:
        print("\n" + "=" * 60)
        print("✗ Setup Failed!")
        print("=" * 60)
        print(f"Error: {e}")
        print("\nTroubleshooting:")
        print("  - Check that the platform and version are correct")
        sys.exit(1)


if __name__ == "__main__":
    main()
