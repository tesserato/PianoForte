Install-Module -Name New-GitHubRelease

New-Item -Path "./Releases/LV2" -ItemType Directory -Force
$lv2ZipPath = "./Releases/LV2/PianoForte.zip"
Compress-Archive -Force -Path "./Releases/LV2 Plugin/PianoForte.lv2" -DestinationPath $lv2ZipPath

$GitHubToken = Get-Content "./GitHubToken.txt"



$note = "Dual engine implemented"

# Import the module dynamically from the PowerShell Gallery. Use CurrentUser scope to avoid having to run as admin.
$version = ""
$jucer = Get-Content -Path './PianoForte.jucer'
ForEach ($line in $jucer) {
  if ($line -like "*pluginName*") {
    if ($line -match '\d\.\d\.\d'){
      $version = $Matches[0]
      Write-Output "Version $version"
      break
    }
    else {
      Write-Output "Couldn't find version"
      exit
    }
  }
}

Import-Module -Name New-GitHubRelease

# Specify the parameters required to create the release. Do it as a hash table for easier readability.
$newGitHubReleaseParameters =
@{
  GitHubUsername       = 'tesserato'
  GitHubRepositoryName = 'PianoForte'
  GitHubAccessToken    = $GitHubToken
  ReleaseName          = "PianoForte v$version"
  TagName              = "v$version"
  ReleaseNotes         = $note
  AssetFilePaths       = @(
    './Releases/Standalone Plugin/PianoForte.exe',
    './Releases/VST3/PianoForte.vst3'
    $lv2ZipPath
  )
  IsPreRelease         = $false
  IsDraft              = $true	# Set to true when testing so we don't publish a real release (visible to everyone) by accident.
}
Write-Output "Creating new release:"
$newGitHubReleaseParameters
# Try to create the Release on GitHub and save the results.
$result = New-GitHubRelease @newGitHubReleaseParameters

# Provide some feedback to the user based on the results.
if ($result.Succeeded -eq $true) {
  Write-Output "Release published successfully! View it at $($result.ReleaseUrl)"
}
elseif ($result.ReleaseCreationSucceeded -eq $false) {
  Write-Error "The release was not created. Error message is: $($result.ErrorMessage)"
}
elseif ($result.AllAssetUploadsSucceeded -eq $false) {
  Write-Error "The release was created, but not all of the assets were uploaded to it. View it at $($result.ReleaseUrl). Error message is: $($result.ErrorMessage)"
}