
function atkSubmit(target)
{
  document.entryform.atkescape.value = target;
  
  // call global submit function, which doesn't get called automatically
  // when we call entryform.submit manually.
  globalSubmit(document.entryform);
  document.entryform.submit();
}
