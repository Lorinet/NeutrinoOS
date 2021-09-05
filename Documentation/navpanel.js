var pages = 
{
    "Introduction": "introduction.html"
};
function showSidePanel(current)
{
    for (var key in pages) 
    {
        if (pages.hasOwnProperty(key))
        {           
            showLink(key, pages[key], current);
        }
    }
}
function showLink(name, href, current)
{
    var ap = '';
    if(name == current) ap = ' style="background-color: #222222;"';
    document.write('<a class="navlink"' + ap + ' href="' + href + '">' + name + '</a>');
}