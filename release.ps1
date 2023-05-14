# Import the module dynamically from the PowerShell Gallery. Use CurrentUser scope to avoid having to run as admin.
Install-Module -Name New-GitHubRelease

Import-Module -Name New-GitHubRelease# -Scope CurrentUser

# Specify the parameters required to create the release. Do it as a hash table for easier readability.
$newGitHubReleaseParameters =
@{
    GitHubUsername = 'tesserato'
    GitHubRepositoryName = 'PianoForte'
    GitHubAccessToken = $Env:GitHubToken
    ReleaseName = "PianoForte v1.0.0"
    TagName = "v1.0.0"
    ReleaseNotes = "This release contains the following changes: ..."
    AssetFilePaths = @(
      './Releases/Standalone Plugin/PianoForte.exe',
      './Releases/VST3/PianoForte.vst3'
    )
    IsPreRelease = $false
    IsDraft = $true	# Set to true when testing so we don't publish a real release (visible to everyone) by accident.
}

# Try to create the Release on GitHub and save the results.
$result = New-GitHubRelease @newGitHubReleaseParameters

# Provide some feedback to the user based on the results.
if ($result.Succeeded -eq $true)
{
    Write-Output "Release published successfully! View it at $($result.ReleaseUrl)"
}
elseif ($result.ReleaseCreationSucceeded -eq $false)
{
    Write-Error "The release was not created. Error message is: $($result.ErrorMessage)"
}
elseif ($result.AllAssetUploadsSucceeded -eq $false)
{
    Write-Error "The release was created, but not all of the assets were uploaded to it. View it at $($result.ReleaseUrl). Error message is: $($result.ErrorMessage)"
}