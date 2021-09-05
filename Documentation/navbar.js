function showNavBar(page)
{
    var html = 
    '<img src="./logo10.png" height="40px"><br><ul>';
    if(page == "Home")
        html += '<li><a class="active" href="../index.php">Home</a></li>';
    else 
        html += '<li><a href="../index.php">Home</a></li>';
    if(page == "News")
        html += '<li><a class="active" href="../news.php">News</a></li>';
    else
        html += '<li><a href="../news.php">News</a></li>';
    if(page == "Download")
        html += '<li><a class="active" href="../download.php">Download</a></li>';
    else
        html += '<li><a href="../download.php">Download</a></li>';
    
    if(page == "Documentation")
        html += '<li><a class="active" href="documentation.php">Documentation</a></li>'; 
    else
        html += '<li><a href="documentation.php">Documentation</a></li>';
    html += '<li><a href="https://github.com/Lorinet/Neutrino">Source code</a></li></ul>';
    document.write(html);
}