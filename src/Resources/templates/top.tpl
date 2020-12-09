{atkconfig var="brand_logo" smartyvar="brand_logo"}
{atkconfig var="dispatcher" smartyvar="dispatcher"}
<div class="main-header navbar navbar-expand navbar-dark navbar-lightblue">

    <!-- Left navbar links -->
    <ul class="navbar-nav">
        <!-- Minimize Sidebar Button -->
        <li class="nav-item">
            <a class="nav-link" data-widget="pushmenu" href="#" role="button"><i class="fas fa-bars"></i></a>
        </li>

        {$menu['left']}

    </ul>

    <!-- Right navbar links -->
    <ul class="navbar-nav ml-auto">

        {$menu['right']}

        <li class="nav-item">
            <a class="nav-link" data-widget="fullscreen" href="#" role="button">
                <i class="fas fa-expand-arrows-alt"></i>
            </a>
        </li>

    </ul>

</div>

<aside class="main-sidebar sidebar-dark-primary elevation-2 sidebar-dark-navy">
    <!-- Brand Logo -->
    <a href="{$dispatcher}" class="brand-link">
        <img src="{$brand_logo}" alt="ATK Logo" class="brand-image img-circle elevation-3"
             style="opacity: .8; max-width: 45px;">
        <span class="brand-text font-weight-light">{$app_title}</span>
    </a>

    <!-- Sidebar -->
    <div class="sidebar">
        <!-- Sidebar Menu -->
        <nav class="mt-2">
            <ul class="nav nav-pills nav-sidebar flex-column nav-child-indent" data-widget="treeview" role="menu" data-accordion="false">
                <!-- Add icons to the links using the .nav-icon class
                     with font-awesome or any other icon font library -->
                {$menu['sidebar']}
            </ul>
        </nav>
        <!-- /.sidebar-menu -->
    </div>
    <!-- /.sidebar -->
</aside>
